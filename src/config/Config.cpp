#include "Config.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>

#include "../utils/Log.h"
#include "../utils/String.h"

namespace Parser {
enum Context {
  None,
  ServerBlock,
  LocationBlock,
};
}

static std::vector<std::string> getValidServerDirectives();
static std::vector<std::string> getValidLocationDirectives();
static bool isServerBlock(const std::vector<std::string>& tokens, Parser::Context currentCtx);
static bool isLocationBlock(const std::vector<std::string>& tokens, Parser::Context currentCtx);
static bool isServerDirective(const std::vector<std::string>& tokens, Parser::Context currentCtx);
static bool isLocationDirective(const std::vector<std::string>& tokens, Parser::Context currentCtx);

const std::string Config::defaultConfigPath = "./conf/default.conf";
const std::vector<std::string> Config::validServerDirectives = getValidServerDirectives();
const std::vector<std::string> Config::validLocationDirectives = getValidLocationDirectives();

void Config::LocationBlock::display() const {
  std::cout << "[location]\n"
      << "uri: " << uri
      << "root: " << root << '\n';
}

Config::Config()
  : Config(defaultConfigPath) {
}

Config::~Config() {
}

Config::Config(const std::string& filePath) {
  if (filePath.empty()) {
    logger.error("No configuration file provided.");
    throw std::exception();
  }
  std::ifstream inf(filePath.c_str());
  if (!inf) {
    logger.error(
      "Unable to open configuration file at "
      + filePath
      + ". Please check if the file exists and if the application has the necessary permissions.");
    throw std::exception();
  }
  if (!parse(inf))
    throw std::exception();
}

bool Config::parse(std::ifstream& inf) {
  std::string line;
  int lineNumber = 0;
  Parser::Context currentCtx = Parser::None;
  std::stack<std::vector<std::string> > locCtx;
  std::stack<std::vector<std::string> > servCtx;
  std::stack<LocationBlock> locs;

  while (std::getline(inf, line)) {
    ++lineNumber;
    line = String::trim(line);

    if (line.empty() || line[0] == '#')
      continue;

    if (line == "}") {
      if (currentCtx == Parser::LocationBlock) {
        LocationBlock loc;
        while (!locCtx.empty()) {
          if (!parseLocationDirective(locCtx.top(), loc))
            return false;
          locCtx.pop();
        }
        locs.push(loc);
        currentCtx = Parser::ServerBlock;
      } else if (currentCtx == Parser::ServerBlock) {
        ServerBlock serv;
        while (!servCtx.empty()) {
          if (!parseServerDirective(servCtx.top(), serv))
            return false;
          servCtx.pop();
        }
        while (!locs.empty()) {
          serv.locations.push_back(locs.top());
          locs.pop();
        }
        conf_.push_back(serv);
        currentCtx = Parser::None;
      }
      continue;
    }
    std::vector<std::string> tokens = String::split(line, ' ');
    if (isServerBlock(tokens, currentCtx)) {
      currentCtx = Parser::ServerBlock;
      continue;
    }
    if (isLocationBlock(tokens, currentCtx)) {
      currentCtx = Parser::LocationBlock;
      continue;
    }
    if (isServerDirective(tokens, currentCtx)) {
      servCtx.push(tokens);
      continue;
    }
    if (isLocationDirective(tokens, currentCtx)) {
      locCtx.push(tokens);
      continue;
    }
    logger.error("Invalid configuration at line " + String::fromInt(lineNumber) + ".");
    return false;
  }
  return true;
}

bool Config::parseServerDirective(const std::vector<std::string>& tokens, ServerBlock& out) {
}

bool Config::parseLocationDirective(const std::vector<std::string>& tokens, LocationBlock& out) {
}

bool isServerBlock(const std::vector<std::string>& tokens, Parser::Context currentCtx) {
  return tokens.size() == 2 && tokens[0] == "server" && tokens[1] == "{" && currentCtx == Parser::None;
}

bool isLocationBlock(const std::vector<std::string>& tokens, Parser::Context currentCtx) {
  return tokens.size() == 3 && tokens[0] == "location" && tokens[2] == "{" && currentCtx == Parser::ServerBlock;
}

bool isServerDirective(const std::vector<std::string>& tokens, Parser::Context currentCtx) {
  if (!(currentCtx == Parser::ServerBlock
      && tokens.size() > 2
      && tokens.back()[tokens.back().length() - 1] == ';'))
      return false;
  for (
    std::vector<std::string>::const_iterator it = Config::validServerDirectives.begin();
    it != Config::validServerDirectives.end();
    ++it) {
    if (tokens[0] == *it)
      return true;
    }
  return false;
}

bool isLocationDirective(const std::vector<std::string>& tokens, Parser::Context currentCtx) {
  if (!(currentCtx == Parser::ServerBlock
      && tokens.size() > 2
      && tokens.back()[tokens.back().length() - 1] == ';'))
    return false;
  for (
    std::vector<std::string>::const_iterator it = Config::validLocationDirectives.begin();
    it != Config::validLocationDirectives.end();
    ++it) {
    if (tokens[0] == *it)
      return true;
  }
  return false;
}

std::vector<std::string> getValidServerDirectives() {
  std::vector<std::string> directives;
  directives.push_back("listen");
  directives.push_back("server_names");
  directives.push_back("error_page");
  directives.push_back("client_max_body_size");
  directives.push_back("root");
  return directives;
}

std::vector<std::string> getValidLocationDirectives() {
  std::vector<std::string> directives;
  directives.push_back("root");
  directives.push_back("autoindex");
  return directives;
}
