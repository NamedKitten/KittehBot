
#include <iostream>
#include <string>
#include <vector>
std::vector<std::string> tokenize( const std::string& str ) {
 std::vector<std::string> result;
 bool inQuote = false;
 std::string current;
 for ( char c : str ) {
  if ( inQuote ) {
   if ( c == '"' ) {
    inQuote = false;
    result.emplace_back( current );
    current = "";
   } else {
    current += c;
   }
  } else {
   if ( c == '"' ) {
    inQuote = true;
   } else if ( c == ' ' ) {
    if ( !current.empty() ) {
     result.emplace_back( current );
     current = "";
    }
   } //
   else {
    current += c;
   }
  }
 }
 return result;
}

namespace Bot {
 namespace Commands {
  namespace Profile {

   namespace Utils {
    json getProfile( std::string GuildID, std::string UserID ) {
     redisclient::RedisValue result;
     result = redis.command( "GET", { "profiles_" + GuildID + "_" + UserID } );

     if ( result.isError() | result.toString() == "" ) {
      redis.command( "SET",
                     { "profiles_" + GuildID + "_" + UserID, "{\"fields\": []}" } );
     }
     std::cout << result.toString() << '\n';
     json parsed;
     try {
      parsed = json::parse( result.toString() );

     } catch ( ... ) {
      redis.command( "SET",
                     { "profiles_" + GuildID + "_" + UserID + "", "{\"fields\": []}" } );
      parsed = json::parse(
          redis.command( "GET", { "profiles_" + GuildID + "_" + UserID } ).toString() );
     }
     return parsed;
    }

    void setProfile( std::string GuildID, std::string UserID, std::string embed_data ) {
     redis.command( "SET", { "profiles_" + GuildID + "_" + UserID, embed_data } );
    }

   } // namespace Utils

   void profiles_command( std::string message, json jmessage, bool jsons = false ) {
    std::string user_id = jmessage["author"]["id"].get<std::string>();
    std::string channel_id = jmessage["channel_id"].get<std::string>();
    json msg = client->sendRestRequest( "GET", "/channels/" + channel_id, {} );
    std::string gid = msg["guild_id"].get<std::string>();
    json embed_data = Bot::Commands::Profile::Utils::getProfile( gid, user_id );
    if ( not jsons ) {
     client->sendRestRequest( "POST", "/channels/" + channel_id + "/messages",
                              { { "embed", embed_data } } );
    } else {
     Embed em;
     em.set_json( embed_data );
     client->sendRestRequest(
         "POST", "/channels/" + channel_id + "/messages",
         { { "content", "```json\n" + em.data.dump( 4 ) + "\n```" } } );
    }
   }

   void setfield_command( std::string message, json jmessage ) {
    std::string user_id = jmessage["author"]["id"].get<std::string>();
    std::string channel_id = jmessage["channel_id"].get<std::string>();
    json msg = client->sendRestRequest( "GET", "/channels/" + channel_id, {} );
    std::string gid = msg["guild_id"].get<std::string>();
    //std::cout << message << '\n';
    std::vector<std::string> sargs = tokenize( message );
    //for(std::string arg : sargs)
    //{
    //    std::cout << arg << '\n';
    //}
    if ( sargs.size() == 2 ) {
     std::string name( sargs[0] );
     std::string value( sargs[1] );
     json embed_data = Bot::Commands::Profile::Utils::getProfile( gid, user_id );
     Embed em;
     em.set_json( embed_data );
     em.add_field( name, value, false );
     // std::cout << em.data.dump(4) << '\n';
     Bot::Commands::Profile::Utils::setProfile( gid, user_id, em.data.dump() );
     client->sendRestRequest( "POST", "/channels/" + channel_id + "/messages",
                              { { "content", "Done!" } } );
    } else {
     client->sendRestRequest(
         "POST", "/channels/" + channel_id + "/messages",
         { { "content", "Correct usage is `" +
                            redis.command( "GET", { "prefix" } ).toString() +
                            "profile setfield \"<name>\" \"<amount>\" `" } } );
    }
   }
   void delfield_command( std::string message, json jmessage ) {
    std::string user_id = jmessage["author"]["id"].get<std::string>();
    std::string channel_id = jmessage["channel_id"].get<std::string>();
    json msg = client->sendRestRequest( "GET", "/channels/" + channel_id, {} );
    std::string gid = msg["guild_id"].get<std::string>();
    //std::cout << message << '\n';
    std::vector<std::string> sargs = tokenize( message );
    //for(std::string arg : sargs)
    //{
    //    std::cout << arg << '\n';
    //}
    if ( sargs.size() == 1 ) {
     std::string name( sargs[0] );
     json embed_data = Bot::Commands::Profile::Utils::getProfile( gid, user_id );
     Embed em;
     em.set_json( embed_data );
     em.delete_field( name );
     // std::cout << em.data.dump(4) << '\n';
     Bot::Commands::Profile::Utils::setProfile( gid, user_id, em.data.dump() );
     client->sendRestRequest( "POST", "/channels/" + channel_id + "/messages",
                              { { "content",
                                  "Done? (If no field existed in the first place then "
                                  "nothing should change.)" } } );
    } else {
     client->sendRestRequest(
         "POST", "/channels/" + channel_id + "/messages",
         { { "content", "Correct usage is `" +
                            redis.command( "GET", { "prefix" } ).toString() +
                            "profile delfield \"<name>\"`" } } );
    }
   }

   void setcolour_command( std::string message, json jmessage ) {
    std::string user_id = jmessage["author"]["id"].get<std::string>();
    std::string channel_id = jmessage["channel_id"].get<std::string>();
    json msg = client->sendRestRequest( "GET", "/channels/" + channel_id, {} );
    std::string gid = msg["guild_id"].get<std::string>();
    //std::cout << message << '\n';
    std::vector<std::string> sargs = tokenize( message );
    //for(std::string arg  : sargs)
    //{
    //    std::cout << arg << '\n';
    //}
    if ( sargs.size() == 1 ) {
     try {
      std::string colour( sargs[0] );
      json embed_data = Bot::Commands::Profile::Utils::getProfile( gid, user_id );
      Embed em;
      em.set_json( embed_data );
      em.set_color( colour );
      Bot::Commands::Profile::Utils::setProfile( gid, user_id, em.data.dump() );
      client->sendRestRequest( "POST", "/channels/" + channel_id + "/messages",
                               { { "content", "Done!" } } );
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
                            "profile setcolour \"<colour>\"`" } } );
    }
   }

   void profile_command( std::string message, json jmessage ) {
    if ( !message.find( "profile setfield" ) ) {
     message = message.substr( 17, message.length() );
     Bot::Commands::Profile::setfield_command( message, jmessage );
    } else if ( !message.find( "profile delfield" ) ) {
     message = message.substr( 17, message.length() );
     Bot::Commands::Profile::delfield_command( message, jmessage );
    } else if ( !message.find( "profile setcolour" ) ) {
     message = message.substr( 18, message.length() );
     Bot::Commands::Profile::setcolour_command( message, jmessage );
    } else if ( !message.find( "profile json" ) ) {
     Bot::Commands::Profile::profiles_command( message, jmessage, true );
    } else if ( !message.find( "profile" ) ) {
     Bot::Commands::Profile::profiles_command( message, jmessage );
    }
   }

  } // namespace Profile
 } // namespace Commands
} // namespace Bot
