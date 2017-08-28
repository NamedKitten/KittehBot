#include "sysconf.h"
#include <boost/algorithm/string.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/timer.hpp>
#include <bot_utils/string_utils.hpp>
#include <conversions.hpp>
#include <csignal>
#include <cstdio>
#include <ctime>
#include <exception>
#include <hexicord/client.hpp>
#include <memory>
#include <redisclient/redissyncclient.h>
#include <spdlog/spdlog.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <vector>

//extern std::shared_ptr<spd::logger> logger;

namespace spd = spdlog;
namespace conversions = NamedKitten::conversions;
using nlohmann::json;
boost::asio::io_service ios;
std::unique_ptr<Hexicord::Client> client;
redisclient::RedisSyncClient redis( ios );

#include <bot_commands/cookies.hpp>
#include <bot_commands/fox.hpp>
#include <bot_commands/ping.hpp>
#include <bot_commands/serverinfo.hpp>
#include <bot_commands/userinfo.hpp>
#include <bot_commands/version.hpp>
#include <bot_utils/chat.hpp>
#include <bot_utils/shell.hpp>
#include <bot_utils/string_utils.hpp>

/*
#include <bot_commands/ddg.hpp>
#include <bot_commands/translate.hpp>
*/

/*
#include "docopt.h"

static const char USAGE[] =
    R"(Usage: KittehBot++ [--reset]

-h --help        show this
--reset          reset settings
)";*/

class Cache {
public:
 json guilds;
 json presences;
};

int main( int argc, const char** argv ) {
 std::vector<spdlog::sink_ptr> sinks;
 auto logger = spdlog::stdout_color_mt( "KittehBot++" );

 std::srand( std::time( 0 ) );
 Cache cache;
 std::signal( SIGABRT, []( int ) { std::exit( 1 ); } );

 std::signal( SIGINT, []( int ) {
  std::exit( 1 );

 } );

 bool needReset = false;

 /*std::map<std::string, docopst::value> args =
      docopt::docopt(USAGE, {argv + 1, argv + argc}, true, "KittehBot++");
  for (auto const& arg : args) {
    if (arg.first == "--reset" && arg.second.asBool()) {
      needReset = true;
    }
  }*/

 logger->info( "Welcome" );

 std::string errmsg;
 if ( !redis.connect( boost::asio::ip::address::from_string( "127.0.0.1" ), 6379,
                      errmsg ) ) {
  logger->error( "Can't connect to redis: " + errmsg + "\n" );
  exit( 1 );
 }

 if ( redis.command( "GET", { "isSetup" } ).toString() == "false" |
      redis.command( "GET", { "isSetup" } ).isNull() ) {
  std::cout << "Welcome to the setup.\n";
  std::cout << "Please enter your token. ";
  std::string tokens;
  std::getline( std::cin, tokens );
  std::cout << "Thanks.\n";
  std::cout << "Please enter your prefix. ";
  std::string prefixs;
  std::getline( std::cin, prefixs );
  std::cout << "Thanks. We will now get you setup.\n" << '\n';

  redis.command( "SET", { "prefix", prefixs } );
  redis.command( "SET", { "token", tokens } );
  redis.command( "SET", { "isSetup", "true" } );
 }

 if ( std::getenv( "TOKEN" ) != NULL ) {
  std::cout << "Using ENV token."
            << "\n";
  redis.command( "SET", { "token", getenv( "TOKEN" ) } );
 }

 if ( std::getenv( "PREFIX" ) != NULL ) {
  std::cout << "Using ENV prefix."
            << "\n";
  redis.command( "SET", { "prefix", getenv( "PREFIX" ) } );
 }
 redisclient::RedisValue result;

 result = redis.command( "GET", { "whitelistedIDs" } );

 if ( result.isError() | result.toString() == "" ) {
  std::string a = "";
  std::cout << "Adding whitelisted IDs list."
            << "\n";
  redis.command( "SET", { "whitelistedIDs", a } );
 }

 std::cout << "Starting bot..."
           << "\n";
 std::string token;
 token = redis.command( "GET", { "token" } ).toString();
 //Hexicord::Clientclient(ios, token);
 client.reset( new Hexicord::Client( ios, token ) );

 client->eventDispatcher.addHandler(
     Hexicord::Event::PresenceUpdate, [&cache]( const nlohmann::json& payload ) {
      cache.presences[payload["user"]["id"].get<std::string>()] = payload;
      if ( payload["status"].is_null() ) {
       cache.presences[payload["user"]["id"].get<std::string>()]["status"] = "offline";
      }

     } );
 client->eventDispatcher.addHandler(
     Hexicord::Event::GuildCreate, [&cache]( const nlohmann::json& payload ) {
      cache.guilds[payload["id"].get<std::string>()] = payload;

      for ( json presence : payload["presences"] ) {
       cache.presences[presence["user"]["id"].get<std::string>()] = presence;
      }
     } );
 client->eventDispatcher.addHandler(
     Hexicord::Event::MessageCreate, [&cache]( const nlohmann::json& payload ) {
      std::string p = redis.command( "GET", { "prefix" } ).toString();
      std::string m = payload["content"].get<std::string>();
      std::string cid = payload["channel_id"].get<std::string>();
      std::string uid = payload["author"]["id"].get<std::string>();

      if ( !m.find( p ) ) {
       auto start = std::chrono::steady_clock::now();
       std::string message = m;
       message = message.substr( p.length(), message.length() );
       std::string sid = payload["channel_id"].get<std::string>();
       if ( !m.find( p + "version" ) ) {
        version_command( payload );
       } else if ( !m.find( p + "fox" ) ) {
        fox_command( payload );
       } else if ( !m.find( p + "ping" ) ) {
        ping_command( payload );
       } else if ( !m.find( p + "dping" ) ) {
        ping_command( payload, true );
       } else if ( !m.find( p + "userinfo" ) ) {
        userinfo_command( payload, cache.presences );
       } else if ( !m.find( p + "serverinfo" ) ) {
        serverinfo_command( payload, cache.guilds );
       } else if ( !m.find( p + "shell" ) ) {
        //shell_command(payload, message, client);
       } else if ( !m.find( p + "set " ) ) {
        //set_command(payload, message, redis, client);
       } else if ( !m.find( p + "cookies" ) ) {
        Bot::Commands::Cookies::cookies_command( message, payload );
       } else if ( !m.find( p + "invite" ) ) {
        json application =
            client->sendRestRequest( "GET", "/oauth2/applications/@me", {} );
        client->sendRestRequest(
            "POST", "/channels/" + cid + "/messages",
            { { "content", "https://discordapp.com/oauth2/authorize?client_id=" +
                               application["id"].get<std::string>() + "&scope=bot" } } );
       } else if ( !m.find( p + "neofetch" ) ) {
        client->sendRestRequest(
            "POST", "/channels/" + cid + "/messages",
            { { "content",
                "```\n" +
                    std::string( shell( "neofetch > a && sed 's/\x1b\[[0-9;]*m//g'" ) +
                                 "\n```" ) } } );
       }
       auto end = std::chrono::steady_clock::now();
       auto elapsed =
           std::chrono::duration_cast<std::chrono::milliseconds>( end - start ).count();
       std::cout << std::to_string( elapsed ) << '\n';
      }
      return std::vector<json>();
     } );

 auto pair = client->getGatewayUrlBot();
 std::cout << "Gateway URL: " << pair.first << '\n';
 std::cout << "Recommended shards count: " << pair.second << '\n';

 std::cout << "Connecting to gateway...\n";
 client->connectToGateway( pair.first );

 ios.run();
}
