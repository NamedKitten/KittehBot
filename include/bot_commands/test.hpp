
void test_command(json jmessage, discordpp::Bot *bot) {
 std::string channel_id =
      jmessage["channel_id"].get<std::string>();
      std::string user_id =
           jmessage["author"]["id"].get<std::string>();
      while (true) {
        bot->call("/channels/300297022437589002/messages", {{"content", "nya"}}, "POST");
      }
      bot->call( "/oauth2/applications/@me", {}, "GET", [user_id, channel_id, jmessage](discordpp::Bot *bot, json msg)
      {
          json application = msg;
          if (application["owner"]["id"].get<std::string>() == user_id)
          {
      std::vector<std::string> words;
    std::string content = jmessage["content"].get<std::string>();
    boost::split(words, content, boost::is_any_of(" "), boost::token_compress_on);


  bot->call(words[1], json::parse(words[2]), words[3], [channel_id, jmessage](discordpp::Bot *bot, json msg){

  bot->call("/channels/" + channel_id + "/messages", {{"content", "```json\n" + msg.dump(4) + "\n```"}}, "POST");

  });
} else {
  bot->call("/channels/" + channel_id + "/messages", {{"content", "Only the bots owner (" + application["owner"]["username"].get<std::string>() + ") can use this command."}}, "POST");

}
});
}
