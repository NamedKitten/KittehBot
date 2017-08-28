
void test_command( json jmessage ) {
 std::string channel_id = jmessage["channel_id"].get<std::string>();
 std::string user_id = jmessage["author"]["id"].get<std::string>();
 json application = client->sendRestRequest( "GET", "/oauth2/applications/@me", {} );
 if ( application["owner"]["id"].get<std::string>() == user_id ) {
  std::vector<std::string> words;
  std::string content = jmessage["content"].get<std::string>();
  boost::split( words, content, boost::is_any_of( " " ), boost::token_compress_on );

  json msg = client->sendRestRequest( words[1], words[2], json::parse( words[3] ) );

  client->sendRestRequest( "POST", "/channels/" + channel_id + "/messages",
                           { { "content", "```json\n" + msg.dump( 4 ) + "\n```" } } );

 } else {
  client->sendRestRequest(
      "POST", "/channels/" + channel_id + "/messages",
      { { "content", "Only the bots owner (" +
                         application["owner"]["username"].get<std::string>() +
                         ") can use this command." } } );
 }
}
