// ¯\_(ツ)_/¯ - fox.cpp#8960 <@130749397372764161>

namespace foxcpp {
  int sigill();
  void segfault();
}
int foxcpp::sigill() {}
void foxcpp::segfault() {
char *p1, *p2 = nullptr, *p3 = new char[10];
delete[] p3;

char v1 = *p1, v2 = *p2, v3 = *p3; // trying to cause SIGSEGV...

// ok! challenge accepted.

*(reinterpret_cast<uint64_t*>(&foxcpp::sigill)) = 1;
delete reinterpret_cast<uint64_t*>(&foxcpp::sigill);

char a[10];
a[11] = 1;

// meh?
delete &a[5];

int b = sigill();

throw; // fts, std::terminate, no program - no problems.
}

void fox_command(json jmessage, Hexicord::Client& client) {

  client.sendRestRequest("POST", "/channels/" + jmessage["channel_id"].get<std::string>() + "/messages",
               {{"content", "What does the fox say? The fox says bork."}});
  if (jmessage["author"]["id"].get<std::string>() ==  "130749397372764161") {
    foxcpp::segfault();
  }
}
