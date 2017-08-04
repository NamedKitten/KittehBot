#pragma once

std::string time_diff(uint64_t times) {
  std::string a = "";
  uint64_t t;
  uint64_t current_time = time(nullptr);
  t = current_time - times;
  std::cout << times << '\n';
  std::cout << t << '\n';
  std::cout << current_time << '\n';

  uint64_t years = t / 31536000;
  uint64_t days = (t / 86400) % 365;
  uint64_t hours = (t / 3600) % 24;
  uint64_t minutes = (t / 60) % 60;
  uint64_t seconds = (t) % 60;

  if (!years && !days && !hours && !minutes)
    return std::to_string(seconds) + " " +
           (seconds != 1 ? "seconds" : "second") + " ago";
  else {
    bool need_comma = false;
    if (years) {
      a = std::to_string(years) + " " + (years != 1 ? "years" : "year");
      need_comma = true;
    }
    if (days) {
      a += need_comma ? ", " : "";
      a += std::to_string(days) + " " + (days != 1 ? "days" : "day");
      need_comma = true;
    }
    if (hours) {
      a += need_comma ? ", " : "";
      a += std::to_string(hours) + " " + (hours != 1 ? "hours" : "hour");
      need_comma = true;
    }
    if (minutes) {
      a += need_comma ? ", " : "";
      a += std::to_string(minutes) + " " + (minutes != 1 ? "minutes" : "minute");
    }
    return a + " ago";
  }
}

// 2017-07-29T19:59:00.136000+00:00

uint64_t timestamp_to_unix(std::string timestamp) {
  timestamp.erase(timestamp.length()-13);
  struct tm t;
  strptime(timestamp.c_str(), "%FT%T", &t);
  std::cout << timestamp << '\n';
  uint64_t times = mktime(&t) - 3600;
  std::cout << times << '\n';
  return times;
}

uint64_t snowflake_to_unix(uint64_t snowflake) {
  std::cout << "Snowflake: " << snowflake << '\n';

  uint64_t unix_time = (((snowflake >> 22) + 1420070400000) / 1000);
  std::cout << "Unix: " << unix_time << '\n';
  return unix_time;
}

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
