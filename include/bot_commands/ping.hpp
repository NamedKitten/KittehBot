void ping_command(std::string sid, std::string cid, discordpp::Bot *bot) {
  std::chrono::steady_clock::time_point begin =
      std::chrono::steady_clock::now();

  json msg = send_message(bot, sid, {{"content", ".."}});
  std::string idb = msg["id"];

  edit_message(bot, sid, idb, {{"content", "..."}});
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  int elapsed =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)
          .count();
  edit_message(
      bot, sid, idb,
      {{"content", "Pong! Time taken to send and edit message: `" +
                       boost::lexical_cast<std::string>(elapsed) + "`ms."}});
}
