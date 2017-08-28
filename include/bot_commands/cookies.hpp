namespace Bot {
 namespace Commands {
  namespace Cookies {

   namespace Utils {
    uint64_t getBalance( std::string GuildID, std::string UserID ) {
     redisclient::RedisValue result;
     result =
         redis.command( "GET", { "cookies_" + GuildID + "_" + UserID + "_balance" } );

     if ( result.isError() | result.toString() == "" ) {
      redis.command( "SET", { "cookies_" + GuildID + "_" + UserID + "_balance", "5" } );
     }
     uint64_t bal = std::strtoull(
         redis.command( "GET", { "cookies_" + GuildID + "_" + UserID + "_balance" } )
             .toString()
             .c_str(),
         nullptr, 10 );
     return bal;
    }
    void takeCookies( std::string GuildID, std::string UserID, uint64_t amount ) {
     uint64_t UserBalance = Bot::Commands::Cookies::Utils::getBalance( GuildID, UserID );

     if ( !( UserBalance >= amount ) ) {
      throw std::runtime_error( "You can't do stuff with cookies that don't exist." );
     }
     redis.command( "SET", { "cookies_" + GuildID + "_" + UserID + "_balance",
                             std::to_string( UserBalance - amount ) } );
    }

    void giveCookies( std::string GuildID, std::string UserID, uint64_t amount ) {
     uint64_t UserBalance = Bot::Commands::Cookies::Utils::getBalance( GuildID, UserID );
     redis.command( "SET", { "cookies_" + GuildID + "_" + UserID + "_balance",
                             std::to_string( UserBalance + amount ) } );
    }

    void giveCookies( std::string GuildID,
                      std::string UserIDFrom,
                      std::string UserIDTo,
                      uint64_t amount ) {
     uint64_t UserFromBalance =
         Bot::Commands::Cookies::Utils::getBalance( GuildID, UserIDFrom );
     uint64_t UserToBalance =
         Bot::Commands::Cookies::Utils::getBalance( GuildID, UserIDTo );
     std::cout << UserFromBalance << '\n';
     std::cout << UserToBalance << '\n';

     if ( !( UserFromBalance >= amount ) ) {
      throw std::runtime_error( "You can't give cookies that don't exist." );
     }

     Bot::Commands::Cookies::Utils::takeCookies( GuildID, UserIDFrom, amount );
     Bot::Commands::Cookies::Utils::giveCookies( GuildID, UserIDTo, amount );
    }

   } // namespace Utils
   void balance_command( std::string message, json jmessage ) {
    std::string user_id = jmessage["author"]["id"].get<std::string>();
    std::string channel_id = jmessage["channel_id"].get<std::string>();
    if ( jmessage["mentions"].size() > 0 ) {
     user_id = jmessage["mentions"][0]["id"].get<std::string>();
    }
    json msg = client->sendRestRequest( "GET", "/channels/" + channel_id, {} );
    std::string gid = msg["guild_id"].get<std::string>();
    uint64_t bal = Bot::Commands::Cookies::Utils::getBalance( gid, user_id );
    client->sendRestRequest(
        "POST", "/channels/" + channel_id + "/messages",
        { { "content",
            "Nya~ " +
                ( user_id == jmessage["author"]["id"] ? "You have `"
                                                      : "<@" + user_id + "> has `" ) +
                std::to_string( bal ) + "` " + ( bal != 1 ? "cookies" : "cookie" ) +
                "." } } );
   }

   void give_command( std::string message, json jmessage ) {
    std::string user_id = jmessage["author"]["id"].get<std::string>();
    std::string channel_id = jmessage["channel_id"].get<std::string>();
    json msg = client->sendRestRequest( "GET", "/channels/" + channel_id, {} );
    std::string gid = msg["guild_id"].get<std::string>();
    uint64_t amount;
    bool found;
    std::string giveto;
    try {
     message = message.substr( 1, message.length() );
    } catch ( ... ) {
     // Must be no args.
    }
    std::vector<std::string> args;
    boost::split( args, message, boost::is_any_of( " " ) );
    if ( jmessage["mentions"].size() == 1 and args.size() == 2 ) {
     giveto = jmessage["mentions"][0]["id"].get<std::string>();
     for ( std::string arg : args ) {
      if ( !arg.find( "<" ) ) {
       // Not a int...
      } else {
       try {
        amount = std::strtoull( arg.c_str(), nullptr, 10 );
        found = true;
       } catch ( ... ) {
        // Must not be a int either
       }
      }
     }
     if ( found ) {
      try {
       Bot::Commands::Cookies::Utils::giveCookies( gid, user_id, giveto, amount );
       client->sendRestRequest(
           "POST", "/channels/" + channel_id + "/messages",
           { { "content", "You gave `" + std::to_string( amount ) + "` " +
                              ( amount != 1 | amount != -1 ? "cookies" : "cookie" ) +
                              " to <@" + giveto + ">." } } );

      } catch ( std::exception const& e ) {
       std::string error( e.what() );
       client->sendRestRequest(
           "POST", "/channels/" + channel_id + "/messages",
           { { "content", "Oh noes... There was a error: " + error } } );
      }
     } else {
      client->sendRestRequest(
          "POST", "/channels/" + channel_id + "/messages",
          { { "content", "Correct usage is `" +
                             redis.command( "GET", { "prefix" } ).toString() +
                             "cookies give (<user> <amount> | <amount> <user>)`" } } );
     }
    } else {
     client->sendRestRequest(
         "POST", "/channels/" + channel_id + "/messages",
         { { "content", "Correct usage is `" +
                            redis.command( "GET", { "prefix" } ).toString() +
                            "cookies give (<user> <amount> | <amount> <user>)`" } } );
    }
   }

   void eat_command( json jmessage ) {
    std::string user_id = jmessage["author"]["id"].get<std::string>();
    std::string channel_id = jmessage["channel_id"].get<std::string>();
    json msg = client->sendRestRequest( "GET", "/channels/" + channel_id, {} );
    std::string gid = msg["guild_id"].get<std::string>();
    try {
     Bot::Commands::Cookies::Utils::takeCookies( gid, user_id, 1 );
     client->sendRestRequest( "POST", "/channels/" + channel_id + "/messages",
                              { { "content", "You ate a :cookie:!" } } );

    } catch ( std::exception const& e ) {
     std::string error( e.what() );
     client->sendRestRequest(
         "POST", "/channels/" + channel_id + "/messages",
         { { "content", "Oh noes... There was a error: " + error } } );
    }
   }

   void cookies_command( std::string message, json jmessage ) {
    if ( !message.find( "cookies balance" ) ) {
     message = message.substr( 15, message.length() );
     Bot::Commands::Cookies::balance_command( message, jmessage );
    } else if ( !message.find( "cookies eat" ) ) {
     Bot::Commands::Cookies::eat_command( jmessage );
    } else if ( !message.find( "cookies give" ) ) {
     message = message.substr( 12, message.length() );
     Bot::Commands::Cookies::give_command( message, jmessage );
    }
   }

  } // namespace Cookies
 } // namespace Commands
} // namespace Bot
