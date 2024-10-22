#pragma once

#include <map>
#include <string>

class Log {
  public:
    enum Level {
      Debug = 0,
      Info,
      Warn,
      Error
    };

    static const std::map<std::string, Level> logLevels;

    Log();
    ~Log();
    void setLogLevel(Level level);
    void setWriteToFile();
    void debug(const std::string& msg) const;
    void info(const std::string& msg) const;
    void warn(const std::string& msg) const;
    void error(const std::string& msg) const;

  private:
    Level logLevel_;
    bool writeToFile_;

    static std::string getTime();
    Log(const Log& other);
    Log& operator=(const Log& other);
};

extern Log logger;
