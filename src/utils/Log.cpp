#include "Log.h"

#include <ctime>

#include <fstream>
#include <iostream>

static std::map<std::string, Log::Level> initializeLogLevels();
static void printLog(const std::string& time, const std::string& level, const std::string& msg);

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

void Log::setWriteToFile(bool writeToFile) {
  writeToFile_ = writeToFile;
}

void Log::setLogFolder(const std::string& folder) {
  logFolder_ = folder;
}

void Log::debug(const std::string& msg) const {
  if (logLevel_ == Debug) {
    if (writeToFile_)
      writeLog(getTime(), "debug", msg);
    else
      printLog(getTime(), "debug", msg);
  }
}

void Log::info(const std::string& msg) const {
  if (logLevel_ <= Info) {
    if (writeToFile_)
      writeLog(getTime(), "info", msg);
    else
      printLog(getTime(), "info", msg);
  }
}

void Log::warn(const std::string& msg) const {
  if (logLevel_ <= Warn) {
    if (writeToFile_)
      writeLog(getTime(), "warn", msg);
    else
      printLog(getTime(), "warn", msg);
  }
}

void Log::error(const std::string& msg) const {
  if (logLevel_ <= Error) {
    if (writeToFile_)
      writeLog(getTime(), "error", msg);
    else
      printLog(getTime(), "error", msg);
  }
}

void Log::writeLog(const std::string& time, const std::string& level, const std::string& msg) const {
#ifdef WEBSERV_DEBUG
  const std::string fullPath = logFolder_ + '/' + "webserv_debug_" + getDate() + ".log";
#else
  const std::string fullPath = logFolder_ + '/' + "webserv_" + getDate() + ".log";
#endif

  std::ofstream outf(fullPath.c_str(), std::ios::app);
  if (outf.is_open()) {
    outf << '[' << time << "] [" << level << "] " << msg << std::endl;
    outf.close();
  } else {
    std::cerr << "Error: unable to write to log file.\n";
  }
}

std::string Log::getDate() {
  std::time_t currentTime = std::time(NULL);
  std::tm* localTime = std::localtime(&currentTime);
  char buffer[80];
  std::strftime(buffer, sizeof buffer, "%Y%m%d", localTime);
  return buffer;
}

std::string Log::getTime() {
  std::time_t currentTime = std::time(NULL);
  std::tm* localTime = std::localtime(&currentTime);
  char buffer[32];
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

void printLog(const std::string& time, const std::string& level, const std::string& msg) {
  std::cout << '[' << time << "] [" << level << "] " << msg << '\n';
}
