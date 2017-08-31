void about_command( json jmessage, json guilds ) {
 json application = client->sendRestRequest( "GET", "/oauth2/applications/@me", {} );
 std::string username = application["name"].get<std::string>();
 std::string channel_id = jmessage["channel_id"].get<std::string>();
 json channel = client->sendRestRequest( "GET", "/channels/" + channel_id, {} );
 std::string guild_id = channel["guild_id"].get<std::string>();
 json guild_json = guilds[guild_id];
 Embed em;
 em.set_author( "About " + username, "",
                "https://cdn.discordapp.com/avatars/" +
                    application["id"].get<std::string>() + "/" +
                    application["icon"].get<std::string>() + ".png" );
 em.add_field( "**Bot version**:", BOT_VERSION );
#ifdef HEXICORD_FULL_VERSION
 em.add_field( "**Hexicord full version**:", HEXICORD_FULL_VERSION );
#else
#ifdef HEXICORD_VERSION
 em.add_field( "**Hexicord version**:", HEXICORD_VERSION );
#endif
#endif
 em.add_field( "**Compiler**:", __VERSION__ );
 em.add_field( "**Prefix**:",
               "`" + redis.command( "GET", { "prefix" } ).toString() + "`" );
 em.set_footer( "Owned by " + application["owner"]["id"].get<std::string>() + "." );
 client->sendRestRequest( "POST", "/channels/" + channel_id + "/messages",
                          { { "embed", em.data } } );
}
