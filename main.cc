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

using json = nlohmann::json;
using boost::system::error_code;
using aios_ptr = std::shared_ptr<asio::io_service>;

std::string shell(std::string cmd) {
std::string data;
FILE * stream;
const int max_buffer = 256;
char buffer[max_buffer];
cmd.append(" 2>&1");

stream = popen(cmd.c_str(), "r");
if (stream) {
while (!feof(stream))
if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
pclose(stream);
}
return data;
}

std::string bold(std::string text) {
 return "**" + text + "**";
}
std::string italic(std::string text) {
 return "*" + text + "*";
}
std::string underline(std::string text) {
 return "__" + text + "__";
}
std::string emlink(std::string name, std::string url) {
 return "[" + name + "](" + url + ")";
}

class Embed
{
 public:

json data = {{"fields", {}}};
void set_json(json stuff)
{
 data = stuff;
}
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

std::string removeMentions(std::string argc, std::string content);
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
    if (m.compare(0, p.length(), p) == 0) {
      std::string message = jmessage["content"].get<std::string>();
      message = message.substr(p.length(), message.length());
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
            bot->call("/channels/" + sid + "/messages",
                      {{"embed", em.data}},
                      "POST");
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
                std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
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
          bot->call(
              "/channels/" + sid, {},
              "GET",
              [sid, jmessage, uid](discordpp::Bot *bot, json msg) {
                std::string gid = msg["guild_id"].get<std::string>();
                json gui = msg;
            Embed em;
            std::string av = jmessage["author"]["avatar"].get<std::string>();
            std::string fu = jmessage["author"]["username"].get<std::string>() + "#" + jmessage["author"]["discriminator"].get<std::string>();
            std::string avs = "https://cdn.discordapp.com/avatars/" + uid + "/" + av + ".png?size=256";
            std::string avl = "https://cdn.discordapp.com/avatars/" + uid + "/" + av + ".png?size=1024";
            std::string un = jmessage["author"]["username"].get<std::string>();
            em.set_thumbnail(avs);
            em.set_author(fu, "", avs);

            json pre = {{"status", "offline"}, {"game", nullptr}};
            std::cout << "Getting pre" << '\n';
            for(json guild : bot->guilds_){
              if (guild["id"].get<std::string>() == gid) {
                for(json pres : guild["presences"]){
                  if (pres["user"]["id"].get<std::string>() == uid) {
                    pre = pres;
                    break;
                  }
                }
              }
            }

            std::string stata = pre["status"].get<std::string>();
            if (stata == "dnd") {
              stata = "Do Not Disturb";
            } else if (stata == "online") {
              stata = "Online";
            } else if (stata == "idle") {
              stata = "Idle";
            } else {
              stata = "Offline";
            }

            json gam = pre["game"];
            std::string game = "";
            if (gam["type"] == 0) {
              game = "Playing " + gam["name"].get<std::string>();
            } else if (gam["type"] == 1){
              game = "Streaming " + gam["name"].get<std::string>();
            } else {
              game = "None";
            }
            em.set_description(
              bold("Display name") + ": " + un + "\n" +
              bold("ID") + ": " + uid + "\n" +
              emlink("Avatar", avl) + "\n" +
              bold("Status") + ": " + stata + "\n" +
              bold("Game") + ": " + game
            );

            json data = em.data;


                  bot->call(
                      "/guilds/" + gid + "/members/" + uid, {},
                      "GET",
                      [sid, jmessage, em, gid, uid, gui, data, pre](discordpp::Bot *bot, json msg) {
                        std::string stata = pre["status"].get<std::string>();
                        Embed em;
                        em.set_json(data);
                        json mem = msg;
                        std::cout << "Member " + mem.dump(4) << "\n";
                        em.add_field("Join dates",
                        bold("This server") + ": " + shell("./time.py " + mem["joined_at"].get<std::string>()) +
                        bold("Discord") + ": " + shell("./time.py " + uid)
                        , true);
                        //if (mem["game"],get<std::string>() == "null") {

          //             std::stringstream roles;
          //for(auto role : mem["roles"]){
            //    roles << "<@&"role.get<std::string>() + "> ";
//}
//                em.add_field("Roles", roles.get<std::string>(), true);
                  bot->call("/channels/" + sid + "/messages",
                            {{"embed", em.data}},
                            "POST");

                });
                });
        } else if (m.compare(0, ge.length(), ge) == 0){
          bot->call(
            "/oauth2/applications/@me",
            {}, "GET", [sid, message, &redis, uid](discordpp::Bot *bot, json msg){

          if (msg["owner"]["id"].get<std::string>() == uid) {
          std::string key = message.substr(4, message.length());
          redisclient::RedisValue result;

          result = redis.command("GET", {key});

            bot->call("/channels/" + sid + "/messages",
                      {{"content", "GET(\"" + key + "\") = \"" + result.toString() + "\"" }},
                      "POST");

          } else {
            bot->call("/channels/" + sid + "/messages",
                      {{"content", "Only the bots owner (" + msg["owner"]["username"].get<std::string>() + ") can use this command." }},
                      "POST");
          }
        });
        } else if (m.compare(0, se.length(), se) == 0){
          bot->call(
            "/oauth2/applications/@me",
            {}, "GET", [sid, message, &redis, uid](discordpp::Bot *bot, json msg){

          if (msg["owner"]["id"].get<std::string>() == uid) {
          std::string keyval = message.substr(4, message.length());
          std::cout << "KeyVal: " + keyval << '\n';
          std::vector< std::string > res;
          boost::algorithm::split(res, keyval, boost::algorithm::is_any_of(" "));
          std::string key = res[0];
          std::cout << "Key: " + key << '\n';
          std::string val = res[1];
          std::cout << "Val: " + val << '\n';

          redisclient::RedisValue result;

          result = redis.command("SET", {key, val});

            bot->call("/channels/" + sid + "/messages",
                      {{"content", "SET(\"" + key + "\", \"" + val + "\") = \"" + result.toString() + "\"" }},
                      "POST");

                    } else {
                        bot->call("/channels/" + sid + "/messages",
                                  {{"content", "Only the bots owner (" + msg["owner"]["username"].get<std::string>() + ") can use this command." }},
                                  "POST");
                      }
                    });
        } else if (m.compare(0, sh.length(), sh) == 0){
          bot->call(
            "/oauth2/applications/@me",
            {}, "GET", [sid, message, &redis, uid](discordpp::Bot *bot, json msg){

          if (msg["owner"]["id"].get<std::string>() == uid) {
          std::string command = message.substr(6, message.length());


            bot->call("/channels/" + sid + "/messages",
                      {{"content", "```shell\n" + shell(command) + "\n```" }},
                      "POST");

                    } else {
                        bot->call("/channels/" + sid + "/messages",
                                  {{"content", "Only the bots owner (" + msg["owner"]["username"].get<std::string>() + ") can use this command." }},
                                  "POST");
                      }
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

          } else if (message == "servers") {
            std::stringstream message;

            for(json guild : bot->guilds_){
              if(guild["unavailable"].get<bool>()){
                message << guild["name"] << " is unavailable.\n";
              } else {
            message << guild["name"] << ".\n";
                  }
                }
                bot->call("/channels/" + sid + "/messages",
                          {{"content", message.str()}}, "POST");



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

  bot.addHandler("READY", [&redis, soft_commands,prefix](discordpp::Bot *bot,
    json jmessage) {

std::cout << "READY!" << '\n';
if (TEST == "yes") {
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
