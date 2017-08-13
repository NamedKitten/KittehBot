#include "sysconf.h"
#include <string>
#include <stdlib.h>
#include <vector>
#include <ctime>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/timer.hpp>
#include <redisclient/redissyncclient.h>
#include <boost/asio/io_service.hpp>
//#include <boost/asio/ip/address.hpp>
namespace asio = boost::asio;
using boost::system::error_code;
using aios_ptr = std::shared_ptr<asio::io_service>;
#include <hexicord/client.hpp>
#include <csignal>
#include <thread>
using nlohmann::json;
#include <conversions.hpp>
namespace conversions = NamedKitten::conversions;
#include <bot_utils/chat.hpp>
#include <bot_utils/shell.hpp>
//#include <bot_utils/bothelper.hpp>
#include <bot_utils/string_utils.hpp>
#include <bot_commands/ping.hpp>
#include <bot_commands/serverinfo.hpp>
#include <bot_commands/userinfo.hpp>
#include <bot_commands/test.hpp>
#include <bot_commands/version.hpp>

/*
#include <bot_commands/fox.hpp>
#include <bot_commands/shell.hpp>
#include <bot_commands/set.hpp>
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
boost::asio::io_service ios;

class Cache {
public:
  json guilds;
  json presences;
};

int main(int argc, const char** argv) {
  Cache cache;
  std::signal(SIGABRT, [](int) {
      std::exit(1);
  });

  std::signal(SIGINT, [](int) {
      std::exit(1);

  });

  bool needReset = false;

  /*std::map<std::string, docopst::value> args =
      docopt::docopt(USAGE, {argv + 1, argv + argc}, true, "KittehBot++");
  for (auto const& arg : args) {
    if (arg.first == "--reset" && arg.second.asBool()) {
      needReset = true;
    }
  }*/

  std::cout << "Welcome." << '\n';
  boost::asio::ip::address address =
      boost::asio::ip::address::from_string("127.0.0.1");

  const unsigned short port = 6379;

  boost::asio::io_service ioService;
  redisclient::RedisSyncClient redis(ioService);
  std::string errmsg;
  if (!redis.connect(address, port, errmsg)) {
    std::cerr << "Can't connect to redis: " << errmsg << std::endl;
    exit(1);
  }

  if (redis.command("GET", {"isSetup"}).toString() ==
       "false") {
    std::cout << "Welcome to the setup.\n";
    std::cout << "Please enter your token. ";
    std::string tokens;
    std::getline(std::cin, tokens);
    std::cout << "Thanks.\n";
    std::cout << "Please enter your prefix. ";
    std::string prefixs;
    std::getline(std::cin, prefixs);
    std::cout << "Thanks. We will now get you setup.\n" << '\n';

    redis.command("SET", {"prefix", prefixs});
    redis.command("SET", {"token", tokens});
    redis.command("SET", {"isSetup", "true"});
  }

  if (std::getenv("TOKEN") != NULL) {
    std::cout << "Using ENV token."
              << "\n";
    redis.command("SET", {"token", getenv("TOKEN")});
  }

  if (std::getenv("PREFIX") != NULL) {
    std::cout << "Using ENV prefix."
              << "\n";
    redis.command("SET", {"prefix", getenv("PREFIX")});
  }
  redisclient::RedisValue result;

  result = redis.command("GET", {"whitelistedIDs"});

if( result.isError() | result.toString() == "" )
{
  std::vector<char> vec = {};
    std::cout << "Adding whitelisted IDs list." << "\n";
    redis.command("SET", {"whitelistedIDs", vec});

}

  std::cout << "Starting bot..."
            << "\n";
  std::string token;
  token = redis.command("GET", {"token"}).toString();
  Hexicord::Client client(ios, token);

  client.eventDispatcher.addHandler(Hexicord::Event::PresenceUpdate, [&cache](const nlohmann::json& payload) {
    cache.presences[payload["user"]["id"].get<std::string>()] = payload;

  });
    client.eventDispatcher.addHandler(Hexicord::Event::GuildCreate, [&cache](const nlohmann::json& payload) {
      cache.guilds[payload["id"].get<std::string>()] = payload;

      for (json presence : payload["presences"]) {
            cache.presences[presence["user"]["id"].get<std::string>()] = presence;

      }
      });
  client.eventDispatcher.addHandler(Hexicord::Event::MessageCreate, [&redis, &client, &cache](const nlohmann::json& payload) {
        std::string p = redis.command("GET", {"prefix"}).toString();
        std::string m = payload["content"].get<std::string>();
        std::string cid = payload["channel_id"].get<std::string>();
        std::string uid = payload["author"]["id"].get<std::string>();
        std::vector<char> ids = redis.command("GET", {"whitelistedIDs"}).toArray();

        if (!m.find(p)) {
          if (!std::find(ids.begin(), ids.end(), uid.c_str()) != ids.end())
          std::chrono::steady_clock::time_point begin =
              std::chrono::steady_clock::now();
          std::string message = payload["content"].get<std::string>();
          message = message.substr(p.length(), message.length());
          std::string sid = payload["channel_id"].get<std::string>();
          if (!m.find(p + "version")) {
            version_command(payload, client);
          } else if (!m.find(p + "test")) {
            test_command(payload, client);
          } else if (!m.find(p + "fox")) {
            //fox_command(payload, client);
          } else if (!m.find(p + "ping")) {
            ping_command(payload, client);
          } else if (!m.find(p + "userinfo")) {
            userinfo_command(payload, client, cache.presences);
          } else if (!m.find(p + "serverinfo")) {
            serverinfo_command(payload, client, cache.guilds);
          } else if (!m.find(p + "shell")) {
            //shell_command(message, sid, uid, sclient);
          } else if (!m.find(p + "set ")) {
            //set_command(payload, message, redis, client);
          } else if (!m.find(p + "ddg ")) {
            //ddg_command(payload, message, client);
          } else if (!m.find(p + "translate ")) {
            //translate_command(payload, message, client);
          }
          std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
          int elapsed =
              std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)
                  .count();
          std::cout << boost::lexical_cast<std::string>(elapsed) << '\n';
        } else {
          if (!m.find(p + "whitelist")) {
            ids.push_back(ids.c_string());
            redis.command("SET", {"whitelistedIDs", ids});
          } else {
            client.sendRestRequest("POST", "/channels/" + channel_id + "/messages/",
            {{"content", "boop"}});
          }
        }
        return std::vector<json>();
      });

      auto pair = client.getGatewayUrlBot();
      std::cout << "Gateway URL: " << pair.first << '\n';
      std::cout << "Recommended shards count: " << pair.second << '\n';

      std::cout << "Connecting to gateway...\n";
      client.connectToGateway(pair.first);

      std::thread second_thread([]() { ios.run(); });
      ios.run();
      second_thread.join();

}
