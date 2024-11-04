#pragma once

#include <map>
#include <string>
#include <vector>

class Config {
  public:
    struct LocationBlock {
      std::string uri;
      std::string root;
      bool autoIndex;
      std::map<std::string, bool> isDirectiveDefined;

      LocationBlock();
      void display() const;
    };

    struct ServerBlock {
      unsigned short port;
      std::string host;
      std::vector<std::string> serverNames;
      size_t clientMaxBodySize;
      std::map<int, std::string> errorPages;
      std::vector<LocationBlock> locations;
      std::string root;
      std::map<std::string, bool> isDirectiveDefined;

      ServerBlock();
      void display() const;
    };

    static const std::string defaultConfigPath;
    static const std::vector<std::string> validServerDirectives;
    static const std::vector<std::string> validLocationDirectives;

    ~Config();
    explicit Config(const std::string& filePath);
    const std::vector<ServerBlock>& getServerConfig() const;

  private:
    std::vector<ServerBlock> conf_;

    bool parse(std::ifstream& inf);
    static bool parseServerDirective(const std::vector<std::string>& tokens, ServerBlock& out);
    static bool parseLocationDirective(const std::vector<std::string>& tokens, LocationBlock& out);

    Config();
    Config(Config& other);
    Config& operator=(Config& other);
};
