void test_command(json jmessage, discordpp::Bot *bot) {
 std::string channel_id =
      jmessage["channel_id"].get<std::string>();
  //std::string a = time_diff(((268563599700525058 >> 22) + 1420070400000) / 1000);
  long a = snowflake_to_unix(175928847299117063);

  bot->call("/channels/" + channel_id + "/messages", {{"content", std::to_string(a)}}, "POST");
}
