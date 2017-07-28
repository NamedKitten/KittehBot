#pragma once
#include <string>
#include <discordpp/bot.hh>
using json = nlohmann::json;

json send_message(discordpp::Bot *bot, std::string channel_id, json j) {
  bool marker = false;
  json msg_;
  bot->call("/channels/" + channel_id + "/messages", j, "POST", [&marker, &msg_](discordpp::Bot *bot, json msg) { marker = true; msg_ = msg; });
  while (!marker);
  return msg_;
}

json edit_message(discordpp::Bot *bot, std::string channel_id, std::string message_id, json j) {
  bool marker = false;
  json msg_;
  bot->call("/channels/" + channel_id + "/messages/" + message_id, j, "PATCH", [&marker, &msg_](discordpp::Bot *bot, json msg) { marker = true; msg_ = msg; });
  while (!marker);
  return msg_;
}

json get_channel(discordpp::Bot *bot, std::string channel_id) {
  bool marker = false;
  json msg_;
  bot->call("/channels/" + channel_id, {}, "GET", [&marker, &msg_](discordpp::Bot *bot, json msg) { marker = true; msg_ = msg; });
  while (!marker);
  return msg_;
}

json get_member(discordpp::Bot *bot, std::string guild_id, std::string user_id) {
  bool marker = false;
  json msg_;
  bot->call("/guilds/" + guild_id + "/members/" + user_id, {}, "GET", [&marker, &msg_](discordpp::Bot *bot, json msg) { marker = true; msg_ = msg; });
  while (!marker);
  return msg_;
}

json get_application(discordpp::Bot *bot) {
  bool marker = false;
  json msg_;
  bot->call("/oauth2/applications/@me", {}, "GET", [&marker, &msg_](discordpp::Bot *bot, json msg) { marker = true; msg_ = msg; });
  while (!marker);
  return msg_;
}
