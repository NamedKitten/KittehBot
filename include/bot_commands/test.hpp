void test_command(json jmessage, discordpp::Bot *bot) {
 std::string channel_id =
      jmessage["channel_id"].get<std::string>();
  //std::string a = time_diff(((268563599700525058 >> 22) + 1420070400000) / 1000);
  std::string a = time_diff(timestamp_to_unix("2017-07-29T19:59:00.136000+00:00"));

  bot->call("/channels/" + channel_id + "/messages", {{"content", a}}, "POST");
}
