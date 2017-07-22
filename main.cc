#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>

#include "sysconf.h"
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/timer.hpp>

namespace asio = boost::asio;

#include <discordpp/bot.hh>
#include <discordpp/rest-curlpp.hh>
#include <discordpp/websocket-websocketpp.hh>
#include <lib/nlohmannjson/src/json.hpp>
#include <string>
#include <trace.h>

using json = nlohmann::json;
using boost::system::error_code;
using aios_ptr = std::shared_ptr<asio::io_service>;

std::string login(std::string authFilePath);
std::map<std::string, std::string> loadSoftCommands(std::string softFilePath);
std::string readTokenFile(std::string tokenFilePath);
// a
int main() {
  std::cout << "Starting bot...\n\n";
  std::string token;
  token = readTokenFile("token.dat");

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
    std::string p("nya++|");
    std::string m = jmessage["content"].get<std::string>();
    std::string aid = jmessage["id"].get<std::string>();
    std::string cid = jmessage["channel_id"].get<std::string>();
    // std::cout << aid + " and " + cid << "\n";
    // std::cout << cha.dump(4) << "\n";
    std::string uid = jmessage["author"]["id"].get<std::string>();
    std::string ech("nya++|echo ");
    std::string sta("nya++|status ");
    if (m.compare(0, p.length(), p) == 0) {
      std::string message = jmessage["content"].get<std::string>();
      message = message.substr(6, message.length());
      std::cout << "\"" + message + "\""
                << "\n";
      auto it = soft_commands.find(message);
      std::string sid = jmessage["channel_id"];
      auto id = boost::lexical_cast<discordpp::snowflake>(sid);
      if (it == soft_commands.end()) {
        if (message == "version") {
          /*json j(bot->call(
              asio_ios, "/oauth2/applications/@me",
              "Bot
             MzM1ODQ2MjEzNDc3ODU5MzQ5.DEvsuQ.nNz84IyV68getasj4oG7qovL7do",
              {}, "GET"));*/
          std::string username = "KittehBot - nya~";
          bot->call(asio_ios, "/channels/" + sid + "/messages",
                    {{"content", "`" + username + "` version `" +
                                     BOT_VERSION "` compiled with `" +
                                     __VERSION__ + "`."}},
                    "POST");
        } else if (message == "ping") {
          //boost::timer myTimer;
          int start_s=clock();
          bot->call(
              asio_ios, "/channels/" + sid + "/messages", {{"content", ".."}},
              "POST",
              [sid, cid, start_s](discordpp::Bot *bot, aios_ptr asio_ios, json msg) {
                std::string idb = msg["id"];

                std::cout << "/channels/" + sid + "/messages/" + idb << "\n";
                bot->call(asio_ios, "/channels/" + sid + "/messages/" + idb,
                          {{"content", "..."}}, "PATCH");
                int stop_s=clock();
                int elapsed = (stop_s-start_s)/double(CLOCKS_PER_SEC)*1000;
                bot->call(asio_ios, "/channels/" + sid + "/messages/" + idb,
                          {{"content", "Pong! Time taken to send and edit message: `" + boost::lexical_cast<std::string>(
                                           elapsed) + "`ms."}},
                          "PATCH");
              });

        } else if (m.compare(0, ech.length(), ech) == 0) {
          std::string mentioncode =
              "<@" + bot->me_["id"].get<std::string>() + ">";
          std::string content = message.substr(4, message.length());
          while (content.find(mentioncode + ' ') != std::string::npos) {
            content = content.substr(0, content.find(mentioncode + ' ')) +
                      content.substr(content.find(mentioncode + ' ') +
                                     (mentioncode + ' ').size());
          }
          while (content.find(mentioncode) != std::string::npos) {
            content =
                content.substr(0, content.find(mentioncode)) +
                content.substr(content.find(mentioncode) + mentioncode.size());
          }
          bot->call(asio_ios, "/channels/" + sid + "/messages",
                    {{"content", content}}, "POST");
        } else if (message == "userinfo") {
          // json embed ({'thumbnail': {'url':
          // 'https://cdn.discordapp.com/avatars/305058659799400448/398ac0171d5b17f7fafe7ac0aee9e5f2.png?size=256'},
          // 'type': 'rich', 'title': '<:online:314049328459022338> NamedKitten -
          // nya~#8468', 'fields': [{'value': '**This server**: 23 days,
          // 5:27:02.038908 ago (2017-06-27 16:24:37.057000)\n**Discord**: 90
          // days, 2:37:26.883921 ago (2017-04-21 19:14:12.212000)', 'inline':
          // True, 'name': 'Join Dates'}, {'value': '<@&220868765242490880>,
          // <@&173469998176534528>', 'inline': True, 'name': 'Roles'}]});
          std::cout << "ye";
          json jsons(R"({
   "thumbnail":{
      "url":"https://cdn.discordapp.com/avatars/305058659799400448/398ac0171d5b17f7fafe7ac0aee9e5f2.png?size=256"
   },
   "title":"NamedKitten - nya~#8468",
   "type":"rich",
   "fields":[
      {
         "inline":true,
         "value":"**This server**: 23 days, 23:09:02.882398 ago \n**Discord**: 90 days, 20:19:27.727413 ago",
         "name":"Join Dates"
      },
      {
         "inline":true,
         "value":"<@&220868765242490880>, <@&173469998176534528>",
         "name":"Roles"
      }
   ],
   "description":"**Display name**: NamedKitten - nya~\n**ID**: 305058659799400448"
})");
          bot->call(asio_ios, "/channels/" + sid + "/messages",
                    {{"content", "hi"}/*, {"embed", jsons}*/}, "POST");

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

std::string readTokenFile(std::string tokenFilePath) {
  std::ifstream tokenFile;
  tokenFile.open(tokenFilePath);

  std::string token;

  if (tokenFile.is_open()) {
    std::getline(tokenFile, token);
  } else {
    std::cerr << "Halp! u fail at leif...";
    exit(1);
  }
  tokenFile.close();

  return token;
}
