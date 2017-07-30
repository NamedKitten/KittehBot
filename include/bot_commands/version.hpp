void version_command(json jmessage, discordpp::Bot *bot) {
  json application = get_application(bot);
  std::string username = application["name"].get<std::string>();
  std::string channel_id = jmessage["channel_id"].get<std::string>();
  send_message(bot, channel_id, {{"content", "`" + username + "` version `" +
                                            BOT_VERSION "` compiled with `" +
                                            __VERSION__ + "` running on `" + shell("uname -a") + "`."}});
}
