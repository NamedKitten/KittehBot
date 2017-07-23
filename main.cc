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


namespace asio = boost::asio;

#include <discordpp/bot.hh>
#include <discordpp/rest-curlpp.hh>
#include <discordpp/websocket-websocketpp.hh>
#include <lib/nlohmannjson/src/json.hpp>
#include <string>

using json = nlohmann::json;
using boost::system::error_code;
using aios_ptr = std::shared_ptr<asio::io_service>;

class Embed
{
 public:
json data = {{"fields", {}}};

 void set_description(std::string description)
 {
  data["description"] = description;
 }
 void set_title(std::string title)
 {
  data["title"] = title;
 }
 void set_timestamp(std::string timestamp)
 {
  data["timestamp"] = timestamp;
 }
 void set_url(std::string url)
 {
  data["url"] = url;
 }
 void set_color(int color)
 {
  data["color"] = color;
 }
 void set_image(std::string url)
 {
  json image = {{"url", url}};
  data["image"] = image;
 }
 void set_thumbnail(std::string url)
 {
  json thumbnail = {{"url", url}};
  data["thumbnail"] = thumbnail;
 }
 void set_footer(std::string text, std::string icon_url = "")
 {
  json footer = {{"text", text}, {"icon_url", icon_url}};
  data["footer"] = footer;
 }
 void add_field(std::string name, std::string value, bool display_inline = false)
 {
  json field = {{"name", name}, {"value", value}, {"inline", display_inline}};
  data["fields"].push_back(field);
 }
 void set_author(std::string name, std::string url = "", std::string icon_url = "")
 {
  json author = {{"name", name}, {"url", url}, {"icon_url", icon_url}};
  data["author"] = author;
 }

};

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
aios_ptr aios = std::make_shared<asio::io_service>();
  discordpp::Bot bot(aios, token, std::make_shared<discordpp::RestCurlPPModule>(aios, token),
                     std::make_shared<discordpp::WebsocketWebsocketPPModule>(aios, token));
  bot.addHandler("MESSAGE_CREATE", [soft_commands](discordpp::Bot *bot,
                                                   json jmessage) {
    std::string p = PREFIX;
    std::string m = jmessage["content"].get<std::string>();
    std::string aid = jmessage["id"].get<std::string>();
    std::string cid = jmessage["channel_id"].get<std::string>();
    std::cout << jmessage.dump(4) << "\n";
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
     "/oauth2/applications/@me",
     {}, "GET", [sid](discordpp::Bot *bot, json msg){
         std::string username = msg["name"].get<std::string>();
         bot->call("/channels/" + sid + "/messages",
                   {{"content", "`" + username + "` version `" +
                                    BOT_VERSION "` compiled with `" +
                                    __VERSION__ + "`."}},
                   "POST");     });

        } else if (message == "test") {
            Embed em;

            em.set_description("this supports [named links](https://discordapp.com) on top of the previously shown subset of markdown. ```\nyes, even code blocks```");
            em.add_field(":thinking:", "some of these properties have certain limits...");
            em.add_field(":scream:", "try exceeding some of them!");
            em.add_field(":rolling_eyes:", "an informative error should show up, and this view will remain as-is until all issues are fixed");
            em.add_field("<:thonkang:219069250692841473>", "these last two", true);
            em.add_field("<:thonkang:219069250692841473>", "are inline fields", true);
            em.set_title("title ~~(did you know you can have markdown here too?)~~");
            em.set_url("https://discordapp.com");
            em.set_color(5034194);
            em.set_timestamp("2017-07-22T18:50:40.383Z");
            em.set_footer("footer text", "https://cdn.discordapp.com/embed/avatars/0.png");
            em.set_thumbnail("https://cdn.discordapp.com/embed/avatars/0.png");
            em.set_image("https://cdn.discordapp.com/embed/avatars/0.png");
            em.set_author("author name", "https://discordapp.com", "https://cdn.discordapp.com/embed/avatars/0.png");
            std::cout << em.data.dump(4) << "\n";
            bot->call("/channels/" + sid + "/messages",
                      {{"embed", em.data}},
                      "POST", [](discordpp::Bot *bot, json msg) {std::cout << msg.dump(4) << "\n";});
        }
         else if (message == "ping") {
          //boost::timer myTimer;
          //int start_s=clock();
          std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();


          bot->call(
              "/channels/" + sid + "/messages", {{"content", ".."}},
              "POST",
              [sid, cid, begin](discordpp::Bot *bot, json msg) {
                std::string idb = msg["id"];

                std::cout << "/channels/" + sid + "/messages/" + idb << "\n";
                bot->call("/channels/" + sid + "/messages/" + idb,
                          {{"content", "..."}}, "PATCH");
                //int stop_s=clock();
                std::chrono::steady_clock::time_point end= std::chrono::steady_clock::now();
                int elapsed = std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count();
                bot->call("/channels/" + sid + "/messages/" + idb,
                          {{"content", "Pong! Time taken to send and edit message: `" + boost::lexical_cast<std::string>(
                                           elapsed) + "`ms."}},
                          "PATCH");
              });

        } else if (m.compare(0, ech.length(), ech) == 0) {
          std::string content = message.substr(4, message.length());
          std::string filtered = removeMentions(content);
          bot->call("/channels/" + sid + "/messages",
                    {{"content", filtered}}, "POST");
        } else if (message == "userinfo") {
            Embed em;
            std::string fu = jmessage["author"]["username"].get<std::string>() + "#" + jmessage["author"]["discriminator"].get<std::string>();
            std::string au = "https://cdn.discordapp.com/avatars/" + uid + "/" + jmessage["author"]["avatar"].get<std::string>() + ".png?size=256";
            em.set_thumbnail(au);
            em.set_author(fu, "", au);

            bot->call(
                "/channels/" + sid, {},
                "GET",
                [sid, jmessage, em, uid](discordpp::Bot *bot, json msg) {
                  std::string gid = msg["guild_id"].get<std::string>();
                  json gui = msg;
                  std::cout << msg.dump(4) << "\n";
                  bot->call(
                      "/guilds/" + gid + "/members/" + uid, {},
                      "GET",
                      [sid, jmessage, em, gid, uid, gui](discordpp::Bot *bot, json msg) {
                        json mem = msg;
                        std::cout << mem.dump(4) << "\n";
            //            std::stringstream roles;
            //for(auto role : mem["roles"]){
            //    roles << "<@&"role.get<std::string>() + "> ";
//}
//                em.add_field("Roles", roles.get<std::string>(), true);
                  std::cout << em.data.dump(4) << "\n";
                  bot->call("/channels/" + sid + "/messages",
                            {{"embed", em.data}},
                            "POST", [](discordpp::Bot *bot, json msg) {std::cout << msg.dump(4) << "\n";});

                });
                });
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
