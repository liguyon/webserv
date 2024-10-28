#include "Config.h"

#include <algorithm>

#include "../utils/Log.h"
#include "../utils/Utils.h"

static bool isTomlExtension(const std::string& filename);
static std::string trimLine(const std::string& line);
static std::vector<std::string> getKnownDirectives();

const std::string Config::defaultConfigPath = "./webserv.toml";
const std::vector<std::string> directives = getKnownDirectives();

Config::Config()
  : Config(defaultConfigPath) {
}

Config::~Config() {
}

Config::Config(const std::string& filePath) {
  if (!parse(filePath))
    throw std::exception();
}

bool Config::parse(const std::string& filePath) {
  if (filePath.length() < 6) {
    logger.error(
      "Invalid configuration filename provided: "
      + filePath
      + ". Please ensure the filename is correct.");
    return false;
  }
  if (!isTomlExtension(filePath)) {
    logger.error(
      "The provided configuration file "
      + filePath
      + "does not have a valid TOML extension. Please ensure the file ends with '.toml'.");
    return false;
  }

  std::ifstream inf(filePath.c_str());
  if (!inf) {
    logger.error(
      "Unable to open configuration file at "
      + filePath
      + ". Please check if the file exists and if the application has the necessary permissions.");
    return false;
  }

  std::string line;
  while (std::getline(inf, line)) {
    line = trimLine(line);
    if (line.empty())
      continue;
    if (line == "[[servers]]") {
      ServerConfig servConf;
      if (!parseServer(inf, servConf)) {
        logger.error(
          "An error occurred while processing the configuration file "
          + filePath
          + ". Please check the file for syntax errors or invalid directives.");
        return false;
      }
      conf_.push_back(servConf);
    } else {
      logger.error(
        "An error occurred while processing the configuration file "
        + filePath
        + ". Please check the file for syntax errors or invalid directives.");
      return false;
    }
  }
  return true;
}

bool Config::parseKvPair(const std::string& line, ServerConfig& serverConf) {
  const std::vector<std::string> tokens = Utils::strSplit(line, '=');
  if (tokens.size() != 2)
    return false;
  const std::string key = Utils::strTrim(tokens[0], ' ');
  const std::string value = Utils::strTrim(tokens[1], ' ');
  if (std::find(directives.begin(), directives.end(), key) == directives.end())
    return false;

  // TODO: implement parse fn for each directive
  if (key == "listen") {
    if (!parseListenDirective(value, serverConf))
      return false;
  } else if (key == "client_max_body_size") {
    if (!parseClientMaxBodySizeDirective(value, serverConf))
      return false;
  } else if (key == "server_names") {
    if (!parseServerNamesDirective(value, serverConf))
      return false;
  }
  return true;
}

bool Config::parseServer(std::ifstream& inf, ServerConfig& parseResult) {
  std::string line;
  while (std::getline(inf, line)) {
    line = trimLine(line);
    // TODO: implement getConfigLineType
    const ConfigLineType type = getConfigLineType();
    switch (type) {
      case typeKvPair:
        if (!parseKvPair(line, parseResult))
          return false;
        break;
      default:
        return false;
    }
  }
  return true;
}

bool isTomlExtension(const std::string& filename) {
  const size_t dotPosition = filename.find_last_of('.');
  if (dotPosition == std::string::npos || dotPosition > filename.length() - 5)
    return false;
  const std::string extension = filename.substr(dotPosition);
  return (extension == ".toml");
}

std::string trimLine(const std::string& line) {
  std::string result;
  const size_t commentPosition = line.find_first_of('#');
  if (commentPosition != std::string::npos)
    result = line.substr(0, commentPosition);
  result = Utils::strTrim(result, ' ');
  return result;
}

std::vector<std::string> getKnownDirectives() {
  std::vector<std::string> knownDirectives;
  knownDirectives.push_back("listen");
  knownDirectives.push_back("client_max_body_size");
  knownDirectives.push_back("server_names");
  knownDirectives.push_back("root");
  knownDirectives.push_back("path");
  return knownDirectives;
}