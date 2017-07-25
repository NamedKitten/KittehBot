#include <string>
#include <discordpp/bot.hh>
#include <lib/nlohmannjson/src/json.hpp>
using json = nlohmann::json;

void ping_command(std::string sid, std::string cid, discordpp::Bot *bot) {
  //boost::timer myTimer;
  //int start_s=clock();
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();


  bot->call(
      "/channels/" + sid + "/messages", {{"content", ".."}},
      "POST",
      [sid, cid, begin](
        discordpp::Bot *bot, json msg) {
        std::string idb = msg["id"];

        std::cout << "/channels/" + sid + "/messages/" + idb << "\n";
        bot->call("/channels/" + sid + "/messages/" + idb,
                  {{"content", "..."}}, "PATCH");
        //int stop_s=clock();
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        int elapsed = std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count();
        bot->call("/channels/" + sid + "/messages/" + idb,
                  {{"content", "Pong! Time taken to send and edit message: `" + boost::lexical_cast<std::string>(
                                   elapsed) + "`ms."}},
                  "PATCH");
      });
}
