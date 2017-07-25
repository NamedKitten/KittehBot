#include <string>
#include <discordpp/bot.hh>
#include <lib/nlohmannjson/src/json.hpp>
#include <bot_utils/chat.hpp>
#include <bot_utils/shell.hpp>
using json = nlohmann::json;

void userinfo_command(std::string sid, std::string uid, json jmessage, discordpp::Bot *bot) {
if (jmessage["mentions"].size() > 0) {
  jmessage["author"] = jmessage["mentions"][0];
  uid = jmessage["author"]["id"];
}
bot->call(
    "/channels/" + sid, {},
    "GET",
    [sid, jmessage, uid](discordpp::Bot *bot, json msg) {
      std::string gid = msg["guild_id"].get<std::string>();
      json gui = msg;
  Embed em;
  std::string av = jmessage["author"]["avatar"].get<std::string>();
  std::string fu = jmessage["author"]["username"].get<std::string>() + "#" + jmessage["author"]["discriminator"].get<std::string>();
  std::string avs = "https://cdn.discordapp.com/avatars/" + uid + "/" + av + ".png?size=256";
  std::string avl = "https://cdn.discordapp.com/avatars/" + uid + "/" + av + ".png?size=1024";
  std::string un = jmessage["author"]["username"].get<std::string>();
  em.set_thumbnail(avs);
  em.set_author(fu, "", avs);

  json pre = {{"status", "offline"}, {"game", nullptr}};
  std::cout << "Getting pre" << '\n';
  for(json guild : bot->guilds_){
    if (guild["id"].get<std::string>() == gid) {
      for(json pres : guild["presences"]){
        if (pres["user"]["id"].get<std::string>() == uid) {
          pre = pres;
          break;
        }
      }
    }
  }

  std::string stata = pre["status"].get<std::string>();
  if (stata == "dnd") {
    stata = "Do Not Disturb";
  } else if (stata == "online") {
    stata = "Online";
  } else if (stata == "idle") {
    stata = "Idle";
  } else {
    stata = "Offline";
  }

  json gam = pre["game"];
  std::string game = "";
  if (gam["type"] == 0) {
    game = "Playing " + gam["name"].get<std::string>();
  } else if (gam["type"] == 1){
    game = "Streaming " + gam["name"].get<std::string>();
  } else {
    game = "None";
  }
  em.set_description(
    bold("Display name") + ": " + un + "\n" +
    bold("ID") + ": " + uid + "\n" +
    emlink("Avatar", avl) + "\n" +
    bold("Status") + ": " + stata + "\n" +
    bold("Game") + ": " + game
  );

  json data = em.data;


        bot->call(
            "/guilds/" + gid + "/members/" + uid, {},
            "GET",
            [sid, jmessage, em, gid, uid, gui, data, pre](discordpp::Bot *bot, json msg) {
              std::string stata = pre["status"].get<std::string>();
              Embed em;
              em.set_json(data);
              json mem = msg;
              std::cout << "Member " + mem.dump(4) << "\n";
              em.add_field("Join dates",
              bold("This server") + ": " + shell("./time.py " + mem["joined_at"].get<std::string>()) +
              bold("Discord") + ": " + shell("./time.py " + uid)
              , true);
              //if (mem["game"],get<std::string>() == "null") {

//             std::stringstream roles;
//for(auto role : mem["roles"]){
  //    roles << "<@&"role.get<std::string>() + "> ";
//}
//                em.add_field("Roles", roles.get<std::string>(), true);
        bot->call("/channels/" + sid + "/messages",
                  {{"embed", em.data}},
                  "POST");

      });
      });
}
