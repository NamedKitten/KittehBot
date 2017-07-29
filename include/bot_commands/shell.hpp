void shell_command(std::string message, std::string sid, std::string uid,
                   discordpp::Bot *bot) {
  json application = get_application(bot);

  if (application["owner"]["id"].get<std::string>() == uid) {
    std::string command = message.substr(6, message.length());

    json ret = send_message(
        bot, sid, {{"content", "```shell\n" + shell(command) + "\n```"}});

  } else {
    send_message(
        bot, sid,
        {{"content", "Only the bots owner (" +
                         application["owner"]["username"].get<std::string>() +
                         ") can use this command."}});
  }
}
