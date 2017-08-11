void translate_command(json jmessage, std::string message, Hexicord::Client *client) {
std::string query = message.substr(10, message.length());
std::string channel_id = jmessage["channel_id"].get<std::string>();

std::stringstream outstream;

cURLpp::Cleanup clean;
//curlpp::Cleanup clean;
curlpp::Easy request;
curlpp::options::WriteStream ws(&outstream);
request.setOpt(ws);
request.setOpt<curlpp::options::Url>("https://translate.yandex.net/api/v1.5/tr.json/translate?key=trnsl.1.1.20170806T034455Z.de2c34cd5bf8ab49.a222cedf6e4d73ccbbf1014f1d3653b0a0a63f29&lang=en&text=" + cURLpp::escape(query));
request.setOpt(curlpp::options::Verbose(false));

request.setOpt(curlpp::options::CustomRequest("POST"));



request.perform();
std::cout << outstream.str() << '\n';
json returned = json::parse(outstream.str());
std::cout << returned.dump(4) << '\n';


client.sendRestRequest("/channels/" + channel_id + "/messages",
{{"content", returned["text"][0].get<std::string>()}}, "POST");


}
