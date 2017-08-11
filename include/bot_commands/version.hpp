void version_command(json jmessage, Hexicord::Client& client) {
  json application = client.sendRestRequest("GET", "/oauth2/applications/@me", {});
  std::string username = application["name"].get<std::string>();
  std::string channel_id = jmessage["channel_id"].get<std::string>();
  client.sendRestRequest("POST", "/channels/" + channel_id + "/messages", {{"content", "`" + username + "` version `" +
                                            BOT_VERSION "` compiled with `" +
                                            __VERSION__ + "` running on `" + shell("uname -a") + "`."}});
}
