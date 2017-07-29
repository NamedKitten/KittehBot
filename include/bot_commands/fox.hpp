// ¯\_(ツ)_/¯ - fox.cpp#8960 <@130749397372764161>

void fox_command(json jmessage, discordpp::Bot *bot) {
  send_message(bot, jmessage["channel_id"].get<std::string>(),
               {{"content", "What does the fox say? The fox says bork."}});
}
