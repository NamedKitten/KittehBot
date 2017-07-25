#include <string>
#include <discordpp/bot.hh>
#include <bot_utils/chat.hpp>

// ¯\_(ツ)_/¯ - fox.cpp#8960 <@130749397372764161>

void fox_command(std::string sid, discordpp::Bot *bot) {
  bot->call("/channels/" + sid + "/messages",
            {{"content", "What does the fox say? The fox says bork."}},
            "POST");
}
