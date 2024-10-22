#include "Log.h"

#include <ctime>
#include <iostream>

static std::map<std::string, Log::Level> initializeLogLevels();
static void printLogMessage(const std::string& time, const std::string& level, const std::string& msg);

Log logger;
const std::map<std::string, Log::Level> Log::logLevels = initializeLogLevels();

Log::Log()
  : logLevel_(Info), writeToFile_(false) {
}

Log::~Log() {
}

void Log::setLogLevel(Level level) {
  logLevel_ = level;
}

void Log::setWriteToFile() {
  writeToFile_ = true;
}

void Log::debug(const std::string& msg) const {
  if (logLevel_ == Debug) {
    if (!writeToFile_)
      printLogMessage(getTime(), "debug", msg);
  }
}

void Log::info(const std::string& msg) const {
  if (logLevel_ <= Info) {
    if (!writeToFile_)
      printLogMessage(getTime(), "info", msg);
  }
}

void Log::warn(const std::string& msg) const {
  if (logLevel_ <= Warn) {
    if (!writeToFile_)
      printLogMessage(getTime(), "warn", msg);
  }
}

void Log::error(const std::string& msg) const {
  if (logLevel_ <= Error) {
    if (!writeToFile_)
      printLogMessage(getTime(), "error", msg);
  }
}

std::string Log::getTime() {
  std::time_t currentTime = std::time(NULL);
  std::tm* localTime = std::localtime(&currentTime);
  char buffer[20];
  std::strftime(buffer, sizeof buffer, "%Y-%m-%d %H:%M:%S", localTime);
  return buffer;
}

std::map<std::string, Log::Level> initializeLogLevels() {
  std::map<std::string, Log::Level> ret;
  ret["debug"] = Log::Debug;
  ret["info"] = Log::Info;
  ret["warn"] = Log::Warn;
  ret["error"] = Log::Error;
  return ret;
}

void printLogMessage(const std::string& time, const std::string& level, const std::string& msg) {
  std::cout << '[' << time << "] [" << level << "] " << msg << '\n';
}
