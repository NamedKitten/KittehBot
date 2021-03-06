#pragma once
#include <string>
using json = nlohmann::json;

std::string bold( std::string text ) { return "**" + text + "**"; }
std::string italic( std::string text ) { return "*" + text + "*"; }
std::string underline( std::string text ) { return "__" + text + "__"; }
std::string emlink( std::string name, std::string url ) {
 return "[" + name + "](" + url + ")";
}

class Embed {
public:
 json data = { { "fields", {} }, { "color", 16755455 } };
 void set_json( json stuff ) { data = stuff; }
 void set_description( std::string description ) { data["description"] = description; }
 void set_title( std::string title ) { data["title"] = title; }
 void set_timestamp( std::string timestamp ) { data["timestamp"] = timestamp; }
 void set_url( std::string url ) { data["url"] = url; }
 void set_color( int color ) { data["color"] = color; }
 void set_color( std::string color ) {
  if ( !color.find( "#" ) ) {
   color = color.substr( 1, color.length() );
  }
  if ( color.length() > 6 ) {
   throw std::runtime_error( "Please input a valid hex code." );
  }
  try {
   std::cout << color << '\n';
   int color_code = std::strtoul( color.c_str(), nullptr, 16 );
   data["color"] = color_code;
   std::cout << color_code << '\n';
  } catch ( std::exception const& e ) {
   std::cout << e.what() << '\n';
   throw std::runtime_error( "Please input a valid hex code." );
  }
 }

 void set_image( std::string url ) {
  json image = { { "url", url } };
  data["image"] = image;
 }
 void set_thumbnail( std::string url ) {
  json thumbnail = { { "url", url } };
  data["thumbnail"] = thumbnail;
 }
 void set_footer( std::string text, std::string icon_url = "" ) {
  json footer = { { "text", text }, { "icon_url", icon_url } };
  data["footer"] = footer;
 }
 void add_field( std::string name, std::string value, bool display_inline = true ) {
  json field = { { "name", name }, { "value", value }, { "inline", display_inline } };
  data["fields"].push_back( field );
 }
 void set_author( std::string name, std::string url = "", std::string icon_url = "" ) {
  json author = { { "name", name }, { "url", url }, { "icon_url", icon_url } };
  data["author"] = author;
 }
 void delete_field( std::string name ) {
  data["fields"].erase( std::remove_if( data["fields"].begin(), data["fields"].end(),
                                        [name]( const json& field ) {
                                         return name == field["name"].get<std::string>();
                                        } ),
                        data["fields"].end() );
 }
};

std::string removeMentions( std::string argc, std::string content );
void scan( std::string& content,
           const unsigned long i,
           const unsigned long oldi,
           const int frontSize );

std::string removeMentions( std::string content ) {
 if ( content.length() >= 21 ) {
  unsigned long i;
  unsigned long oldi;
  for ( i = content.size() - 22, oldi = i + 1; i < oldi;
        i = std::min( i - 1, content.size() - 22 ) ) {
   if ( content.substr( i, 3 ) == "<@!" ) {
    scan( content, i, oldi, 3 );
   } else if ( content.substr( i, 2 ) == "<@" ) {
    scan( content, i, oldi, 2 );
   }
   oldi = i;
  }
 }
 return content;
}

void scan( std::string& content,
           const unsigned long i,
           const unsigned long oldi,
           const int frontSize ) {
 bool isMention = true;
 for ( int n = i + frontSize; n < i + frontSize + 18 && isMention; n++ ) {
  isMention = isdigit( content.at( n ) );
 }
 if ( isMention && content.at( i + frontSize + 18 ) == '>' ) {
  if ( content.size() > i + frontSize + 19 && content.at( i + frontSize + 19 ) == ' ' ) {
   content = content.substr( 0, i ) + content.substr( i + frontSize + 20 );
  } else {
   content = content.substr( 0, i ) + content.substr( i + frontSize + 19 );
  }
 }
}
