#include "Config.h"

#include <cstdlib>

#include <fstream>
#include <iostream>
#include <stack>

#include "../utils/Log.h"
#include "../utils/String.h"

namespace Parser {
enum Context {
  None,
  ServerBlock,
  LocationBlock
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

Config::LocationBlock::LocationBlock(): autoIndex(false) {
  for (
    std::vector<std::string>::const_iterator it = validLocationDirectives.begin();
    it != validLocationDirectives.end();
    ++it) {
    isDirectiveDefined[*it] = false;
  }
}

void Config::LocationBlock::display() const {
  std::cout << "[location]\n"
      << "uri: " << uri << '\n'
      << "root: " << root << '\n'
      << "autoindex: " << (autoIndex ? "true" : "false") << '\n';
}

Config::ServerBlock::ServerBlock(): port(0), clientMaxBodySize(0) {
  for (
    std::vector<std::string>::const_iterator it = validServerDirectives.begin();
    it != validServerDirectives.end();
    ++it) {
    isDirectiveDefined[*it] = false;
  }
}

void Config::ServerBlock::display() const {
  std::cout << "[server]\n"
      << "addr: " << host << ":" << String::fromInt(port) << '\n'
      << "root: " << root << '\n'
      << "client_max_body_size: " << clientMaxBodySize << '\n';
  for (std::map<int, std::string>::const_iterator it = errorPages.begin(); it != errorPages.end(); ++it) {
    std::cout << "error_page: " << it->first << " " << it->second << '\n';
  }
  std::cout << "server_name: ";
  for (std::vector<std::string>::const_iterator it = serverNames.begin(); it != serverNames.end(); ++it) {
    std::cout << *it << ' ';
  }
  std::cout << '\n';
  for (std::vector<LocationBlock>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
    it->display();
  }
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
  if (!parse(inf)) {
    logger.error(std::string("The configuration file provided is invalid.")
      + " Please check the file format and ensure all required parameters are correctly specified.");
    throw std::exception();
  }
}

const std::vector<Config::ServerBlock>& Config::getServerConfig() const {
  return conf_;
}

bool Config::parse(std::ifstream& inf) {
  std::string line;
  Parser::Context currentCtx = Parser::None;
  std::stack<std::vector<std::string> > locCtx;
  std::stack<std::vector<std::string> > servCtx;
  std::stack<LocationBlock> locs;

  while (std::getline(inf, line)) {
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
        if (!loc.isDirectiveDefined["root"])
          return false;
        locs.push(loc);
        currentCtx = Parser::ServerBlock;
      } else if (currentCtx == Parser::ServerBlock) {
        ServerBlock serv;
        while (!servCtx.empty()) {
          if (!parseServerDirective(servCtx.top(), serv))
            return false;
          servCtx.pop();
        }
        if (
          !serv.isDirectiveDefined["listen"]
          || !serv.isDirectiveDefined["root"]
          || !serv.isDirectiveDefined["server_name"])
          return false;
        while (!locs.empty()) {
          serv.locations.insert(serv.locations.begin(), locs.top());
          locs.pop();
        }
        conf_.push_back(serv);
#ifdef WEBSERV_DEBUG
        serv.display();
#endif
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
      locCtx.push(tokens);
      continue;
    }
    if (isServerDirective(tokens, currentCtx)) {
      std::string last = tokens.back().substr(0, tokens.back().length() - 1);
      tokens.pop_back();
      tokens.push_back(last);
      servCtx.push(tokens);
      continue;
    }
    if (isLocationDirective(tokens, currentCtx)) {
      std::string last = tokens.back().substr(0, tokens.back().length() - 1);
      tokens.pop_back();
      tokens.push_back(last);
      locCtx.push(tokens);
      continue;
    }
    return false;
  }
  return true;
}

bool Config::parseServerDirective(const std::vector<std::string>& tokens, ServerBlock& out) {
  // parse listen
  if (tokens[0] == "listen" && !out.isDirectiveDefined[tokens[0]]) {
    if (tokens.size() != 2)
      return false;
    std::vector<std::string> addr = String::split(tokens[1], ':');
    if (addr.size() != 2)
      return false;
    const long port = std::strtol(addr[1].c_str(), NULL, 10);
    if (!(port >= 1024 && port <= 65535))
      return false;
    out.host = addr[0];
    out.port = static_cast<unsigned short>(port);
    out.isDirectiveDefined[tokens[0]] = true;
    return true;
  }
  // parse root
  if (tokens[0] == "root" && !out.isDirectiveDefined[tokens[0]]) {
    if (tokens.size() != 2)
      return false;
    out.root = tokens[1];
    out.isDirectiveDefined[tokens[0]] = true;
    return true;
  }
  // parse server_name
  if (tokens[0] == "server_name" && !out.isDirectiveDefined[tokens[0]]) {
    std::vector<std::string> sn = tokens;
    sn.erase(sn.begin());
    out.serverNames = sn;
    out.isDirectiveDefined[tokens[0]] = true;
    return true;
  }
  // parse client_max_body_size
  if (tokens[0] == "client_max_body_size" && !out.isDirectiveDefined[tokens[0]]) {
    const size_t kilo = 1024;
    const size_t mega = kilo * 1024;
    const size_t giga = mega * 1024;

    if (tokens.size() != 2)
      return false;
    char suffix = 0;
    std::string nstr = tokens[1];
    if (std::isalpha(nstr[nstr.length() - 1])) {
      suffix = nstr[nstr.length() - 1];
      nstr = tokens[1].substr(0, tokens[1].length() - 1);
    }
    if (suffix && suffix != 'K' && suffix != 'M' && suffix != 'G')
      return false;
    for (std::string::const_iterator it = nstr.begin(); it != nstr.end(); ++it)
      if (!(*it >= '0' && *it <= '9'))
        return false;
    const long nb = std::strtol(nstr.c_str(), NULL, 10);
    if (nb < 1 || static_cast<size_t>(nb) > giga)
      return false;
    size_t n = static_cast<size_t>(nb);
    if (suffix && suffix == 'K')
      n *= kilo;
    else if (suffix && suffix == 'M')
      n *= mega;
    else
      n *= giga;
    if (n > giga)
      return false;
    out.clientMaxBodySize = n;
    out.isDirectiveDefined[tokens[0]] = true;
    return true;
  }
  // parse error_page
  if (tokens[0] == "error_page") {
    if (tokens.size() != 3)
      return false;
    long nb = std::strtol(tokens[1].c_str(), NULL, 10);
    if (nb < 100 || nb > 599)
      return false;
    int errcode = static_cast<int>(nb);
    if (out.errorPages.find(errcode) != out.errorPages.end())
      return false;
    out.errorPages[errcode] = tokens[2];
    return true;
  }
  return false;
}

bool Config::parseLocationDirective(const std::vector<std::string>& tokens, LocationBlock& out) {
  // parse uri
  if (tokens[0] == "location") {
    out.uri = tokens[1];
    return true;
  }
  // parse root
  if (tokens[0] == "root" && !out.isDirectiveDefined[tokens[0]]) {
    if (tokens.size() != 2)
      return false;
    out.root = tokens[1];
    out.isDirectiveDefined[tokens[0]] = true;
    return true;
  }
  if (tokens[0] == "autoindex" && !out.isDirectiveDefined[tokens[0]]) {
    if (tokens[1] == "on")
      out.autoIndex = true;
    else if (tokens[1] == "off")
      out.autoIndex = false;
    else
      return false;
    out.isDirectiveDefined[tokens[0]] = true;
    return true;
  }
  return false;
}

bool isServerBlock(const std::vector<std::string>& tokens, Parser::Context currentCtx) {
  return tokens.size() == 2 && tokens[0] == "server" && tokens[1] == "{" && currentCtx == Parser::None;
}

bool isLocationBlock(const std::vector<std::string>& tokens, Parser::Context currentCtx) {
  return tokens.size() == 3 && tokens[0] == "location" && tokens[2] == "{" && currentCtx == Parser::ServerBlock;
}

bool isServerDirective(const std::vector<std::string>& tokens, Parser::Context currentCtx) {
  if (!(currentCtx == Parser::ServerBlock
    && tokens.size() >= 2
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
  if (!(currentCtx == Parser::LocationBlock
    && tokens.size() >= 2
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
  directives.push_back("server_name");
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
