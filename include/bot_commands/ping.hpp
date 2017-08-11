void ping_command(json jmessage, Hexicord::Client& client) {
  std::string channel_id = jmessage["channel_id"].get<std::string>();
  std::chrono::steady_clock::time_point begin =
      std::chrono::steady_clock::now();

  json msg = client.sendRestRequest("POST", "/channels/" + channel_id + "/messages", {{"content", ".."}});
  std::string new_message = msg["id"];

  client.sendRestRequest("PATCH", "/channels/" + channel_id + "/messages/" + new_message, {{"content", "..."}});
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  int elapsed =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)
          .count();
  client.sendRestRequest("PATCH", "/channels/" + channel_id + "/messages/" + new_message,
      {{"content", "Pong! Time taken to send and edit message: `" +
                       boost::lexical_cast<std::string>(elapsed) + "`ms."}});
}
