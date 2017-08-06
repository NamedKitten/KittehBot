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
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <discordpp/bot.hh>
#include <discordpp/rest-curlpp.hh>
#include <discordpp/websocket-websocketpp.hh>
using nlohmann::json;
#include <conversions.hpp>
namespace conversions = NamedKitten::conversions;

#include <bot_utils/chat.hpp>
#include <bot_utils/shell.hpp>
#include <bot_utils/bothelper.hpp>
#include <bot_utils/string_utils.hpp>
#include <bot_commands/fox.hpp>
#include <bot_commands/ping.hpp>
#include <bot_commands/shell.hpp>
#include <bot_commands/test.hpp>
#include <bot_commands/userinfo.hpp>
#include <bot_commands/serverinfo.hpp>
#include <bot_commands/version.hpp>
#include <bot_commands/set.hpp>
#include <bot_commands/ddg.hpp>
#include <bot_commands/translate.hpp>

/*
#include "docopt.h"

static const char USAGE[] =
    R"(Usage: KittehBot++ [--reset]

-h --help        show this
--reset          reset settings
)";*/

int main(int argc, const char** argv) {
  bool needReset = false;

  /*std::map<std::string, docopt::value> args =
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

  std::cout << "Starting bot..."
            << "\n";
  std::string token;
  token = "Bot " + redis.command("GET", {"token"}).toString();
  std::cout << "a" << '\n';
  aios_ptr aios = std::make_shared<asio::io_service>();
  discordpp::Bot bot(
      aios, token, std::make_shared<discordpp::RestCurlPPModule>(aios, token),
      std::make_shared<discordpp::WebsocketWebsocketPPModule>(aios, token));
    std::cout << "b" << '\n';
  bot.addHandler(
      "MESSAGE_CREATE", [&redis](discordpp::Bot* bot, json jmessage) {
        std::string prefix = redis.command("GET", {"prefix"}).toString();
        std::string p = prefix;
        std::string m = jmessage["content"].get<std::string>();
        std::string cid = jmessage["channel_id"].get<std::string>();
        std::string uid = jmessage["author"]["id"].get<std::string>();
        if (!m.find(p)) {
          std::chrono::steady_clock::time_point begin =
              std::chrono::steady_clock::now();
          std::string message = jmessage["content"].get<std::string>();
          message = message.substr(p.length(), message.length());
          std::string sid = jmessage["channel_id"].get<std::string>();
          if (!m.find(p + "version")) {
            version_command(jmessage, bot);
          } else if (!m.find(p + "test")) {
            test_command(jmessage, bot);
          } else if (!m.find(p + "fox")) {
            fox_command(jmessage, bot);
          } else if (!m.find(p + "ping")) {
            ping_command(jmessage, bot);
          } else if (!m.find(p + "userinfo")) {
            userinfo_command(jmessage, bot);
          } else if (!m.find(p + "serverinfo")) {
            serverinfo_command(jmessage, bot);
          } else if (!m.find(p + "shell")) {
            shell_command(message, sid, uid, bot);
          } else if (!m.find(p + "set ")) {
            set_command(jmessage, message, redis, bot);
          } else if (!m.find(p + "ddg ")) {
            ddg_command(jmessage, message, bot);
          } else if (!m.find(p + "translate ")) {
            translate_command(jmessage, message, bot);
          }
          std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
          int elapsed =
              std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)
                  .count();
          std::cout << boost::lexical_cast<std::string>(elapsed) << '\n';
        }
        return std::vector<json>();
      });

  bot.addHandler("READY", [](discordpp::Bot* bot, json jmessage) {
    if (TEST == "yes") {
      exit(0);
    }
  });

  aios->run();
}
