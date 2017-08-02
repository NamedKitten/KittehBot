void test_command(json jmessage, discordpp::Bot *bot) {
  std::string channel_id = jmessage["channel_id"].get<std::string>();
  Embed em;
  em.set_description(
      "this supports [named links](https://discordapp.com) on top of the "
      "previously shown subset of markdown. ```\nyes, even code blocks```");
  em.add_field(":thinking:", "some of these properties have certain limits...");
  em.add_field(":scream:", "try exceeding some of them!");
  em.add_field(":rolling_eyes:",
               "an informative error should show up, and this view will remain "
               "as-is until all issues are fixed");
  em.add_field("<:thonkang:219069250692841473>", "these last two", true);
  em.add_field("<:thonkang:219069250692841473>", "are inline fields", true);
  em.set_title("title ~~(did you know you can have markdown here too?)~~");
  em.set_url("https://discordapp.com");
  em.set_color(5034194);
  em.set_timestamp("2017-07-22T18:50:40.383Z");
  em.set_footer("footer text",
                "https://cdn.discordapp.com/embed/avatars/0.png");
  em.set_thumbnail("https://cdn.discordapp.com/embed/avatars/0.png");
  em.set_image("https://cdn.discordapp.com/embed/avatars/0.png");
  em.set_author("author name", "https://discordapp.com",
                "https://cdn.discordapp.com/embed/avatars/0.png");
  bot->call("/channels/" + channel_id + "/messages", {{"embed", em.data}},
            "POST");
}
