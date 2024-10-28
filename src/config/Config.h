#pragma once

#include <fstream>
#include <map>
#include <string>
#include <vector>


class Config {
  public:
    static const std::string defaultConfigPath;
    static const std::vector<std::string> directives;

    Config();
    ~Config();
    explicit Config(const std::string& filePath);

  private:
    struct LocationDirective {
      std::string path;
      std::string root;
    };
    struct ServerConfig {
      std::string listen;
      std::vector<std::string> serverNames;
      int clientMaxBodySize;
      std::map<int, std::string> errorPages;
      std::vector<LocationDirective> locations;
    };
    enum ConfigLineType {
      typeKvPair,
      typeTable,
      typeError
    };

    std::vector<ServerConfig> conf_;

    bool parse(const std::string& filePath);
    static bool parseKvPair(const std::string& line, ServerConfig& serverConf);
    static bool parseServer(std::ifstream& inf, ServerConfig& parseResult);
    Config(Config& other);
    Config& operator=(Config& other);
};
