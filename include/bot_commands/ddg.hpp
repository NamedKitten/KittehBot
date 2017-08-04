void ddg_command(json jmessage, std::string message, discordpp::Bot *bot) {
std::string query = message.substr(4, message.length());
std::string channel_id = jmessage["channel_id"].get<std::string>();

std::stringstream outstream;

cURLpp::Cleanup clean;
//curlpp::Cleanup clean;
curlpp::Easy request;
curlpp::options::WriteStream ws(&outstream);
request.setOpt(ws);
request.setOpt<curlpp::options::Url>("http://api.duckduckgo.com/?q=" + query + "&format=json");
request.setOpt(curlpp::options::Verbose(false));

request.setOpt(curlpp::options::CustomRequest("GET"));



request.perform();

json returned = json::parse(outstream.str());
std::cout << returned.dump(4) << '\n';

Embed em;
if (returned["Type"].get<std::string>() == "A") {
  em.add_field("Answer", returned["AbstractText"].get<std::string>());
  std::cout << em.data.dump(4) << '\n';
  std::string source = "DuckDuckGo";
  if (!returned["AbstractSource"].is_null()) {
    source = returned["AbstractSource"].get<std::string>();
  }
  if (!returned["Image"].is_null()) {
    em.set_thumbnail(returned["Image"].get<std::string>());
  }
  em.add_field("Source", "[" + source + "](" + returned["AbstractURL"].get<std::string>() + ")");
  std::cout << em.data.dump(4) << '\n';

} else if (returned["Type"].get<std::string>() == "") {
    em.add_field("Answer", returned["RelatedTopics"][0]["Text"].get<std::string>());
    em.add_field("Source", "[Site]({" + returned["RelatedTopics"][0]["FirstURL"].get<std::string>() + "})");
} else if (returned["Type"].get<std::string>() == "D") {
    em.add_field("Answer", returned["RelatedTopics"][0]["Text"].get<std::string>());
    em.add_field("Source", "[Site]({" + returned["RelatedTopics"][0]["FirstURL"].get<std::string>() + "})");
}

bot->call("/channels/" + channel_id + "/messages",
{{"embed", em.data}}, "POST");


}
