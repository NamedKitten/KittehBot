void set_command(json jmessage, std::string message, redisclient::RedisSyncClient &redis, Hexicord::Client *client)
{
    std::string user_id = jmessage["author"]["id"].get<std::string>();
    std::string channel_id = jmessage["channel_id"].get<std::string>();

    client.sendRestRequest( "/oauth2/applications/@me", {}, "GET", [user_id, channel_id, message, &redis, jmessage](Hexicord::Client *client, json msg)
    {
        json application = msg;
        if (application["owner"]["id"].get<std::string>() == user_id)
        {
            std::string command = message.substr(4, message.length());

            if (!command.find("prefix "))
            {
                std::string prefix = command.substr(7, message.length());
                redis.command("SET", {"prefix", prefix});
                std::cout << prefix << '\n';

                client.sendRestRequest("/channels/" + channel_id + "/messages",
                    {{"content", "The prefix has been set to `" + prefix + "`."}},
                    "POST");
            }

            if (!command.find("name ") or !command.find("username "))
            {
                std::string username;
                if (!command.find("name "))
                    username = command.substr(5, message.length());
                else
                    username = command.substr(9, message.length());


                client.sendRestRequest("/users/@me", {{"username", username}}, "PATCH",
                [channel_id, username](Hexicord::Client *client, json msg1)
                {
                    std::cout << msg1.dump(4) << '\n';
                    client.sendRestRequest(
                        "/channels/" + channel_id + "/messages",
                        {{"content", "My name is now `" + username + "`."}},
                        "POST");
                });
            }
            else
            {
                client.sendRestRequest("/channels/" + channel_id + "/messages",
                {{"content", "Only the bots owner (" + application["owner"]["username"].get<std::string>() + ") can use this command."}}, "POST");
            }
        }
    });
}
