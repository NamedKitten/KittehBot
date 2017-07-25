#include <string>
#include <discordpp/bot.hh>
#include <lib/nlohmannjson/src/json.hpp>
using json = nlohmann::json;

void shell_command(std::string message, std::string sid, std::string uid, discordpp::Bot *bot) {
  bot->call(
    "/oauth2/applications/@me",
    {}, "GET", [sid, message, uid](discordpp::Bot *bot, json msg){

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
}
