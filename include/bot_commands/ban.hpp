void ban_command(json jmessage, std::string message, discordpp::Bot *bot) {
std::string user_id = jmessage["author"]["id"].get<std::string>();
std::string channel_id = jmessage["channel_id"].get<std::string>();
bot->call("/channels/" + channel_id, {}, "GET", [user_id, channel_id, jmessage, message](discordpp::Bot *bot, json msg) {
std::string gid = msg["guild_id"].get<std::string>();
std::cout << msg.dump(4) << '\n';
bot->call("/guilds/" + gid + "/bans",
{}, "GET", [user_id, channel_id, jmessage, message](discordpp::Bot *bot, json msg) {
  if (!msg["code"].get<std::string>() == "50013") {
    bot->call("/channels/" + channel_id + "/messages",
    {{"content", "I don't have permissions to ban this person..."}}, "POST");
  }

bot->call("/channels/" + channel_id + "/messages",
{{"content", "potato"}}, "POST");
});
});
}
