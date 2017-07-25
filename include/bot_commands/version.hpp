#include <string>
#include <discordpp/bot.hh>
#include <lib/nlohmannjson/src/json.hpp>
using json = nlohmann::json;

void version_command(std::string sid, discordpp::Bot *bot) {
  bot->call(
  "/oauth2/applications/@me",
  {}, "GET", [sid](discordpp::Bot *bot, json msg){
  std::string username = msg["name"].get<std::string>();
  bot->call("/channels/" + sid + "/messages",
         {{"content", "`" + username + "` version `" +
                          BOT_VERSION "` compiled with `" +
                          __VERSION__ + "`."}},
         "POST");     });
}
