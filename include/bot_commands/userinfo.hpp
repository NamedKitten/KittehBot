void userinfo_command(json jmessage,
                      discordpp::Bot *bot) {
  std::string user_id = jmessage["author"]["id"].get<std::string>();
  std::string channel_id = jmessage["channel_id"].get<std::string>();
  json wait_message = send_message(bot, channel_id, {{"content", "Please wait..."}});
  if (jmessage["mentions"].size() > 0) {
    jmessage["author"] = jmessage["mentions"][0];
    user_id = jmessage["author"]["id"];
  }

  std::string gid = get_channel(bot, channel_id)["guild_id"].get<std::string>();
  Embed em;
    std::string username = jmessage["author"]["username"].get<std::string>();
  std::string avatar = jmessage["author"]["avatar"].get<std::string>();
  std::string full_name = username + "#" +
                   jmessage["author"]["discriminator"].get<std::string>();
  std::string avatar_small =
      "https://cdn.discordapp.com/avatars/" + user_id + "/" + avatar + ".png?size=256";
  std::string avatar_large =
      "https://cdn.discordapp.com/avatars/" + user_id + "/" + avatar + ".png?size=1024";
  em.set_thumbnail(avatar_small);
  em.set_author(full_name, "", avatar_small);

  json pre = {{"status", "offline"}, {"game", nullptr}};
  for (json guild : bot->guilds_) {
    if (guild["id"].get<std::string>() == gid) {
      for (json pres : guild["presences"]) {
        if (pres["user"]["id"].get<std::string>() == user_id) {
          pre = pres;
          break;
        }
      }
    }
  }

  std::string stata = pre["status"].get<std::string>();
  if (stata == "dnd") {
    stata = "Do Not Disturb";
  } else if (stata == "online") {
    stata = "Online";
  } else if (stata == "idle") {
    stata = "Idle";
  } else {
    stata = "Offline";
  }

  json gam = pre["game"];
  std::string game = "";
  if (gam["type"] == 0) {
    game = "Playing " + gam["name"].get<std::string>();
  } else if (gam["type"] == 1) {
    game = "Streaming " + gam["name"].get<std::string>();
  } else {
    game = "None";
  }
  em.set_description(bold("Display name") + ": " + username + "\n" + bold("ID") +
                     ": " + user_id + "\n" + emlink("Avatar", avatar_large) + "\n" +
                     bold("Status") + ": " + stata + "\n" + bold("Game") +
                     ": " + game);
  json mem = get_member(bot, gid, user_id);
  em.add_field("Join dates",
               bold("This server") + ": " +
                   shell("./time.py " + mem["joined_at"].get<std::string>()) +
                   bold("Discord") + ": " + shell("./time.py " + user_id),
               true);
  edit_message(bot, channel_id, wait_message["id"].get<std::string>(), {{"embed", em.data}, {"content", nullptr}});
}
