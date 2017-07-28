#pragma once
#include <string>
#include <discordpp/bot.hh>
using json = nlohmann::json;

std::string bold(std::string text) {
 return "**" + text + "**";
}
std::string italic(std::string text) {
 return "*" + text + "*";
}
std::string underline(std::string text) {
 return "__" + text + "__";
}
std::string emlink(std::string name, std::string url) {
 return "[" + name + "](" + url + ")";
}

class Embed
{
 public:

json data = {{"fields", {}}};
void set_json(json stuff)
{
 data = stuff;
}
 void set_description(std::string description)
 {
  data["description"] = description;
 }
 void set_title(std::string title)
 {
  data["title"] = title;
 }
 void set_timestamp(std::string timestamp)
 {
  data["timestamp"] = timestamp;
 }
 void set_url(std::string url)
 {
  data["url"] = url;
 }
 void set_color(int color)
 {
  data["color"] = color;
 }
 void set_image(std::string url)
 {
  json image = {{"url", url}};
  data["image"] = image;
 }
 void set_thumbnail(std::string url)
 {
  json thumbnail = {{"url", url}};
  data["thumbnail"] = thumbnail;
 }
 void set_footer(std::string text, std::string icon_url = "")
 {
  json footer = {{"text", text}, {"icon_url", icon_url}};
  data["footer"] = footer;
 }
 void add_field(std::string name, std::string value, bool display_inline = false)
 {
  json field = {{"name", name}, {"value", value}, {"inline", display_inline}};
  data["fields"].push_back(field);
 }
 void set_author(std::string name, std::string url = "", std::string icon_url = "")
 {
  json author = {{"name", name}, {"url", url}, {"icon_url", icon_url}};
  data["author"] = author;
 }

};

std::string removeMentions(std::string argc, std::string content);
void scan(std::string& content, const unsigned long i, const unsigned long oldi, const int frontSize);

std::string removeMentions(std::string content){
  if(content.length() >= 21){
    unsigned long i;
    unsigned long oldi;
    for(i = content.size() - 22, oldi = i + 1; i < oldi; i = std::min(i - 1, content.size() - 22)){
      if(content.substr(i, 3) == "<@!"){
	scan(content, i, oldi, 3);
      }else if(content.substr(i, 2) == "<@"){
	scan(content, i, oldi, 2);
      }
      oldi = i;
    }
  }
  return content;
}

void scan(std::string& content, const unsigned long i, const unsigned long oldi, const int frontSize){
  bool isMention = true;
  for(int n = i + frontSize; n < i + frontSize + 18 && isMention; n++){
    isMention = isdigit(content.at(n));
  }
  if(isMention && content.at(i + frontSize + 18) == '>'){
    if(content.size() > i + frontSize + 19 && content.at(i + frontSize + 19) == ' '){
      content = content.substr(0, i) + content.substr(i + frontSize + 20);
    }else{
      content = content.substr(0, i) + content.substr(i + frontSize + 19);
    }
  }
}
