void ping_command(json jmessage, discordpp::Bot *bot) {
  std::string channel_id = jmessage["channel_id"].get<std::string>();
  std::chrono::steady_clock::time_point begin =
      std::chrono::steady_clock::now();

  json msg = send_message(bot, channel_id, {{"content", ".."}});
  std::string new_message = msg["id"];

  edit_message(bot, channel_id, new_message, {{"content", "..."}});
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  int elapsed =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)
          .count();
  edit_message(
      bot, channel_id, new_message,
      {{"content", "Pong! Time taken to send and edit message: `" +
                       boost::lexical_cast<std::string>(elapsed) + "`ms."}});
}
