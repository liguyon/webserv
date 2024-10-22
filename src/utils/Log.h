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
    void setWriteToFile(bool writeToFile);
    void setLogFolder(const std::string& folder);
    void debug(const std::string& msg) const;
    void info(const std::string& msg) const;
    void warn(const std::string& msg) const;
    void error(const std::string& msg) const;

  private:
    Level logLevel_;
    bool writeToFile_;
    std::string logFolder_;

    void writeLog(const std::string& time, const std::string& level, const std::string& msg) const;
    static std::string getDate();
    static std::string getTime();
    Log(const Log& other);
    Log& operator=(const Log& other);
};

extern Log logger;
