void serverinfo_command(json jmessage,
                      Hexicord::Client& client, json guilds) {
  std::string channel_id = jmessage["channel_id"].get<std::string>();
  json msg = client.sendRestRequest("GET", "/channels/" + channel_id, {});
  std::string gid = msg["guild_id"].get<std::string>();
  Embed em;
  std::cout << "na" << '\n';
  json guild_json = guilds[gid];
  int verification_level_int = guild_json["verification_level"];
  std::string verification_level = "";
  if (verification_level_int  == 0) {
    verification_level = "None";
  } else if (verification_level_int == 1) {
    verification_level = "Low";
  } else if (verification_level_int == 2) {
    verification_level = "Medium";
  } else if (verification_level_int == 3) {
    verification_level = "(╯°□°）╯︵ ┻━┻";
  } else {
    verification_level = "┻━┻ミヽ(ಠ益ಠ)ﾉ彡┻━┻ ";
  }
  if (!guild_json["icon"].is_null()) {
    std::string icon_url = "https://cdn.discordapp.com/icons/" + gid + "/" + guild_json["icon"].get<std::string>() + ".jpg";
    em.set_description("**ID**: " + guild_json["id"].get<std::string>() + "\n[Icon URL](" + icon_url + ")");
    em.set_thumbnail(icon_url);
  } else {
    em.set_description("**ID**: " + guild_json["id"].get<std::string>());
  }
  json roles = guild_json["roles"];
  std::sort(roles.begin(), roles.end(), [](const auto& p1, const auto& p2) {
      return p1["position"] > p2["position"];
  });

  std::string roles_str;

  for (const auto& subjson : roles) {
    if (! (subjson["name"].get<std::string>() == "@everyone")) {
      roles_str +=  "<@&" + subjson["id"].get<std::string>() + ">";
      roles_str += ", ";
    }
  }

  if (boost::algorithm::ends_with(roles_str, ", ")) {
    roles_str.erase(roles_str.length()-2);
  }

  json channels = guild_json["channels"];
  std::sort(channels.begin(), channels.end(), [](const auto& p1, const auto& p2) {
      return p1["position"] < p2["position"];
  });

  std::string channels_str;

  for (const auto& subjson : channels) {
      if (subjson["type"].get<int>() == 0) {
      channels_str +=  "<#" + subjson["id"].get<std::string>() + ">";
      channels_str += ", ";
    }
  }

  if (boost::algorithm::ends_with(channels_str, ", ")) {
    channels_str.erase(channels_str.length()-2);
  }
  em.add_field("Text channels", channels_str);
  em.add_field("Members", std::to_string(guild_json["member_count"].get<int>()));
  em.add_field("Roles", roles_str);
  int explicit_content_filter_int = guild_json["explicit_content_filter"];
  std::string explicit_level = "";
  if (explicit_content_filter_int == 0) {
    explicit_level = "Don't scan any messages";
  } else if (explicit_content_filter_int == 1) {
    explicit_level = "Scan messages from members without a role";
  } else {
    explicit_level = "Scan messages sent by all members";
  }
  std::string info = "";
  if (!guild_json["afk_channel_id"].is_null()) {
  info += "**AFK channel**: <#" + guild_json["afk_channel_id"].get<std::string>() + ">\n";
  info += "**AFK timeout**: " + std::to_string(guild_json["afk_timeout"].get<int>()) + " seconds\n";
  }
  info += "**Owner**: <@" + guild_json["owner_id"].get<std::string>() + ">";
  info += "\n**Region**: `" + guild_json["region"].get<std::string>() + "`\n";

  info += "**Verification level**: " + verification_level + "\n**Explicit content filter**: " + explicit_level;

  em.add_field("Other miscellaneous info", info);

  client.sendRestRequest("POST", "/channels/" + channel_id + "/messages", {{"embed", em.data}, {"content", nullptr}});

}
