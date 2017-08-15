void shell_command(json jmessage, std::string message, Hexicord::Client& client) {
  std::string user_id = jmessage["author"]["id"].get<std::string>();
  std::string channel_id = jmessage["channel_id"].get<std::string>();
  json application = client.sendRestRequest("GET", "/oauth2/applications/@me", {});

  if (application["owner"]["id"].get<std::string>() == user_id) {

    std::string command = message.substr(6, message.length());

    client.sendRestRequest("POST", "/channels/" + channel_id + "/messages", {{"content", shell(command)}});
  } else {
   client.sendRestRequest("POST", "/channels/" + channel_id + "/messages",
         {{"content", "Only the bots owner (" + application["owner"]["username"].get<std::string>() + ") can use this command."}});
  }
}
