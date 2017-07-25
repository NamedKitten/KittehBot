#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>
#include <ctype.h>
#include <chrono>
#include "sysconf.h"
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/timer.hpp>
#include <boost/thread/thread.hpp>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
namespace asio = boost::asio;
#include <discordpp/bot.hh>
#include <discordpp/rest-curlpp.hh>
#include <discordpp/websocket-websocketpp.hh>
#include <lib/nlohmannjson/src/json.hpp>
#include <string>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/address.hpp>
#include <redisclient/redissyncclient.h>
#include <bot_utils/chat.hpp>
#include <bot_utils/shell.hpp>
#include <bot_commands/ping.hpp>
#include <bot_commands/test.hpp>
#include <bot_commands/version.hpp>
#include <bot_commands/shell.hpp>
#include <bot_commands/fox.hpp>
#include <bot_commands/userinfo.hpp>

using json = nlohmann::json;
using boost::system::error_code;
using aios_ptr = std::shared_ptr<asio::io_service>;

std::string login(std::string authFilePath);
std::map<std::string, std::string> loadSoftCommands(std::string softFilePath);

int main(int argc, char *argv[] ) {
  bool needReset = false;
  if( argc > 1 ) {
     needReset = true;
}
std::cout << "Welcome." << '\n';
boost::asio::ip::address address = boost::asio::ip::address::from_string("127.0.0.1");

const unsigned short port = 6379;

boost::asio::io_service ioService;
redisclient::RedisSyncClient redis(ioService);
std::string errmsg;
if( !redis.connect(address, port, errmsg) )
{
    std::cerr << "Can't connect to redis: " << errmsg << std::endl;
    return EXIT_FAILURE;
}

if ((redis.command("GET", {"isSetup"}).toString() == "false") or needReset ) {
  std::cout << "Welcome to the setup.\n";
  std::cout << "Please enter your token. ";
  std::string tokens;
  std::getline(std::cin, tokens);
  tokens = "Bot " + tokens;
  boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
  std::cout << "Thanks.\n";
  boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
  std::cout << "\nPlease enter your prefix. \t";
  std::string prefixs;
  std::getline(std::cin, prefixs);

  boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
  std::cout << "Thanks. We will now get you setup.\n" << '\n';

  redis.command("SET", {"prefix", prefixs});
  redis.command("SET", {"token", tokens});
  redis.command("SET", {"isSetup", "true"});

}

  std::cout << "Starting bot...\n\n";
  std::string token;
token = redis.command("GET", {"token"}).toString();
  std::map<std::string, std::string> soft_commands =
      loadSoftCommands("softcommands.dat");
std::string prefix = redis.command("GET", {"prefix"}).toString();
  std::cout << "soft commands\n";
  for (auto elem : soft_commands) {
    std::cout << elem.first << "\n" << elem.second << "\n\n";
  }
aios_ptr aios = std::make_shared<asio::io_service>();
  discordpp::Bot bot(aios, token, std::make_shared<discordpp::RestCurlPPModule>(aios, token),
                     std::make_shared<discordpp::WebsocketWebsocketPPModule>(aios, token));
  bot.addHandler("MESSAGE_CREATE", [&redis, soft_commands,prefix](discordpp::Bot *bot,
                                                   json jmessage) {
    std::string p = prefix;
    std::string m = jmessage["content"].get<std::string>();
    std::string aid = jmessage["id"].get<std::string>();
    std::string cid = jmessage["channel_id"].get<std::string>();
    std::string uid = jmessage["author"]["id"].get<std::string>();
    std::string ech = p + "echo ";
    std::string sta = p + "status ";
    std::string ge = p + "get ";
    std::string se = p + "set ";
    std::string sh = p + "shell ";
    std::string uifo = p + "userinfo";
    if (m.compare(0, p.length(), p) == 0) {
      std::string message = jmessage["content"].get<std::string>();
      message = message.substr(p.length(), message.length());
      auto it = soft_commands.find(message);
      std::string sid = jmessage["channel_id"].get<std::string>();
      if (it == soft_commands.end()) {
        if (message == "version") {
          version_command(sid, bot);
        } else if (message == "test") {
          test_command(sid, bot);
        }
        else if (message == "fox") {
          fox_command(sid, bot);
        }
         else if (message == "ping") {
           ping_command(sid, cid, bot);

        } else if (m.compare(0, uifo.length(), uifo) == 0) {
          userinfo_command(sid, uid, jmessage, bot);
        } else if (m.compare(0, sh.length(), sh) == 0){
            shell_command(message, sid, uid, bot);
        }
      } else {

        bot->call("/channels/" + sid + "/messages",
                  {{"content", it->second}}, "POST");
      }
    }
    return std::vector<json>();
  });

  // bot->addHandler("PRESENCE_UPDATE", [](discordpp::Bot *bot, json jmessage)
  // { ignore
  //});
  // bot->addHandler("TYPING_START", [](discordpp::Bot *bot, json jmessage) {
  // ignore
  //});

  bot.addHandler("READY", [&redis, soft_commands,prefix, aios](discordpp::Bot *bot,
    json jmessage) {

std::cout << "READY!" << '\n';
if (TEST == "yes") {
  aios->stop();
  exit(0);
}
});
  aios->run();
}
std::map<std::string, std::string> loadSoftCommands(std::string softFilePath) {
  std::ifstream softfile;
  softfile.open(softFilePath);

  std::map<std::string, std::string> soft_commands;

  if (softfile.is_open()) {
    while (!softfile.eof()) {
      std::string command;
      std::getline(softfile, command);
      std::string blurb; // Currently unused.
      std::getline(softfile, blurb);
      std::string output;
      std::getline(softfile, output);
      if (output == "{") {
        output = "";
        std::string nextline;
        std::getline(softfile, nextline);
        while (nextline != "}") {
          output += nextline;
          std::getline(softfile, nextline);
          if (nextline != "}") {
            output += "\n";
          }
        }
      }
      soft_commands[command] = output;
    }
  } else {
    std::cerr << "PlsAddsoftcommands.dat";
  }
  softfile.close();

  return soft_commands;
}
