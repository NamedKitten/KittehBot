void ping_command( json jmessage, bool dping = false ) {
 std::string channel_id = jmessage["channel_id"].get<std::string>();
 auto starts = std::chrono::steady_clock::now();
 json msg = client->sendRestRequest( "POST", "/channels/" + channel_id + "/messages",
                                     { { "content", ".." } } );
 std::string new_message = msg["id"];
 if ( dping ) {
  client->sendRestRequest( "PATCH",
                           "/channels/" + channel_id + "/messages/" + new_message,
                           { { "content", "..." } } );
 }
 auto ends = std::chrono::steady_clock::now();
 std::string dmessage;
 if ( dping ) {
  dmessage = "and edit ";
 }
 auto elapseds =
     std::chrono::duration_cast<std::chrono::milliseconds>( ends - starts ).count();
 client->sendRestRequest(
     "PATCH", "/channels/" + channel_id + "/messages/" + new_message,
     { { "content", "Pong! Time taken to send " + dmessage + "message: `" +
                        std::to_string( elapseds ) + "`ms." } } );
}
