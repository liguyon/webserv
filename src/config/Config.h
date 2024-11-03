#pragma once

#include <map>
#include <string>
#include <vector>

class Config {
  public:
    struct LocationBlock {
      std::string uri;
      std::string root;

      void display() const;
    };

    struct ServerBlock {
      unsigned short port;
      std::string host;
      std::vector<std::string> serverNames;
      int clientMaxBodySize;
      std::map<int, std::string> errorPages;
      std::vector<LocationBlock> locations;
      std::string root;
      bool autoIndex;
      void display() const;
    };

    static const std::string defaultConfigPath;
    static const std::vector<std::string> validServerDirectives;
    static const std::vector<std::string> validLocationDirectives;

    Config();
    ~Config();
    explicit Config(const std::string& filePath);

  private:
    std::vector<ServerBlock> conf_;

    bool parse(std::ifstream& inf);
    bool parseServerDirective(const std::vector<std::string>& tokens, ServerBlock& out);
    bool parseLocationDirective(const std::vector<std::string>& tokens, LocationBlock& out);

    Config(Config& other);
    Config& operator=(Config& other);
};
