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
#include <boost/regex.hpp>


namespace asio = boost::asio;

#include <discordpp/bot.hh>
#include <discordpp/rest-curlpp.hh>
#include <discordpp/websocket-websocketpp.hh>
#include <lib/nlohmannjson/src/json.hpp>
#include <string>

using json = nlohmann::json;
using boost::system::error_code;
using aios_ptr = std::shared_ptr<asio::io_service>;

std::string login(std::string authFilePath);
std::map<std::string, std::string> loadSoftCommands(std::string softFilePath);

std::string removeMentions(std::string content);
void scan(std::string& content, const unsigned long i, const unsigned long oldi, const int frontSize);

std::string removeMentions(std::string content){
  unsigned long i;
  unsigned long oldi;
  for(i = content.size() - 22, oldi = i + 1; i < oldi; i = std::min(i - 1, content.size() - 22)){
    if(content.substr(i, 3) == "<@!"){
      scan(content, i, oldi, 3);
    }else if(content.substr(i, 2) == "<@"){
      scan(content, i, oldi, 2);
    }
    oldi = i;
  }
  return content;
}

void scan(std::string& content, const unsigned long i, const unsigned long oldi, const int frontSize){
  bool isMention = true;
  for(int n = i + frontSize; n < i + frontSize + 18 && isMention; n++){
    isMention = isdigit(content.at(n));
  }
  if(isMention && content.at(i + frontSize + 18) == '>'){
    if(content.size() > i + frontSize + 19 && content.at(i + frontSize + 19) == ' '){
      content = content.substr(0, i) + content.substr(i + frontSize + 20);
    }else{
      content = content.substr(0, i) + content.substr(i + frontSize + 19);
    }
  }
}

int main() {
  std::cout << "Starting bot...\n\n";
  std::string token;
  token = TOKEN;

  std::map<std::string, std::string> soft_commands =
      loadSoftCommands("softcommands.dat");

  std::cout << "soft commands\n";
  for (auto elem : soft_commands) {
    std::cout << elem.first << "\n" << elem.second << "\n\n";
  }

  discordpp::Bot bot(token, std::make_shared<discordpp::RestCurlPPModule>(),
                     std::make_shared<discordpp::WebsocketWebsocketPPModule>());
  bot.addHandler("MESSAGE_CREATE", [soft_commands](discordpp::Bot *bot,
                                                   aios_ptr asio_ios,
                                                   json jmessage) {
    std::string p = PREFIX;
    std::string m = jmessage["content"].get<std::string>();
    std::string aid = jmessage["id"].get<std::string>();
    std::string cid = jmessage["channel_id"].get<std::string>();
    // std::cout << aid + " and " + cid << "\n";
    // std::cout << cha.dump(4) << "\n";
    std::string uid = jmessage["author"]["id"].get<std::string>();
    std::string ech = p + "echo ";
    std::string sta = p + "status ";
    if (m.compare(0, p.length(), p) == 0) {
      std::string message = jmessage["content"].get<std::string>();
      message = message.substr(p.length(), message.length());
      std::cout << "\"" + message + "\""
                << "\n";
      auto it = soft_commands.find(message);
      std::string sid = jmessage["channel_id"];
      auto id = boost::lexical_cast<discordpp::snowflake>(sid);
      if (it == soft_commands.end()) {
        if (message == "version") {
            bot->call(
     asio_ios, "/oauth2/applications/@me",
     {}, "GET", [sid](discordpp::Bot *bot, aios_ptr asio_ios, json msg){
         std::string username = msg["name"].get<std::string>();
         bot->call(asio_ios, "/channels/" + sid + "/messages",
                   {{"content", "`" + username + "` version `" +
                                    BOT_VERSION "` compiled with `" +
                                    __VERSION__ + "`."}},
                   "POST");     });

        } else if (message == "ping") {
          //boost::timer myTimer;
          //int start_s=clock();
          std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();


          bot->call(
              asio_ios, "/channels/" + sid + "/messages", {{"content", ".."}},
              "POST",
              [sid, cid, begin](discordpp::Bot *bot, aios_ptr asio_ios, json msg) {
                std::string idb = msg["id"];

                std::cout << "/channels/" + sid + "/messages/" + idb << "\n";
                bot->call(asio_ios, "/channels/" + sid + "/messages/" + idb,
                          {{"content", "..."}}, "PATCH");
                //int stop_s=clock();
                std::chrono::steady_clock::time_point end= std::chrono::steady_clock::now();
                int elapsed = std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count();
                bot->call(asio_ios, "/channels/" + sid + "/messages/" + idb,
                          {{"content", "Pong! Time taken to send and edit message: `" + boost::lexical_cast<std::string>(
                                           elapsed) + "`ms."}},
                          "PATCH");
              });

        } else if (m.compare(0, ech.length(), ech) == 0) {
          std::string content = message.substr(4, message.length());
          std::string filtered = removeMentions(content);
          bot->call(asio_ios, "/channels/" + sid + "/messages",
                    {{"content", filtered}}, "POST");
        } else if (message == "userinfo") {
            //pass
        } else if (m.compare(0, sta.length(), sta) == 0) {
            std::string status = message.substr(7, message.length());
            std::cout << "\"" + status + "\""
                      << "\n";
          bot->send(
              3, {{"game",
                   "null"},
                  {"status", status},
                  {"afk", false},
                  {"since", "null"}});

}
      } else {

        bot->call(asio_ios, "/channels/" + sid + "/messages",
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

  aios_ptr asio_ios = std::make_shared<asio::io_service>();
  bot.init(asio_ios);
  asio_ios->run();
  return 0;
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
