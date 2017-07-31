void set_command(json jmessage, std::string message, redisclient::RedisSyncClient& redis,
                   discordpp::Bot *bot) {
  std::string user_id = jmessage["author"]["id"].get<std::string>();
  std::string channel_id = jmessage["channel_id"].get<std::string>();
  bot->call("/oauth2/applications/@me", {}, "GET", [user_id, channel_id, message, &redis, jmessage](discordpp::Bot *bot, json msg) {
  json application = msg;
  if (application["owner"]["id"].get<std::string>() == user_id) {
    std::string command = message.substr(4, message.length());
    if (!command.find("prefix ")) {
      std::string prefix = message.substr(11, message.length());
      redis.command("SET", {"prefix", prefix});
      std::cout << prefix << '\n';
      bot->call("/channels/" + channel_id + "/messages", {{"content", "The prefix has been set."}}, "POST");

    }

  } else {
    bot->call("/channels/" + channel_id + "/messages", {{"content", "Only the bots owner (" +
                     application["owner"]["username"].get<std::string>() +
                     ") can use this command."}}, "POST");

  }
});

}