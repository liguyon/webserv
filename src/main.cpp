#include <cstdlib>

#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include "utils/Log.h"
#include "server/Server.h"

static void usage(const std::string& msg = "", int exitStatus = 0) {
  if (!msg.empty())
    std::cout << msg << "\n\n";

  std::cout << "usage: webserv [OPTIONS] CONFIGURATION_FILE\n\n"
      << "Options:\n"
      << "  -h, --help             Print this help text and exit.\n\n"
      << "  --log-file <folder>    Write logs into log files located in the specified folder.\n"
      << "                         If this option is not provided, logs will be written to the standard output.\n\n"
      << "  --log-level <level>    Set the logging level for the application.\n"
      << "                         Valid levels are: debug, info, warn, error.\n"
      << "                         If this option is not provided, the default level is set to 'info'.\n";
  exit(exitStatus);
}

static std::vector<std::string> getCommandLineArguments(char* argv[]) {
  std::vector<std::string> args;
  while (*argv) {
    args.push_back(*argv);
    ++argv;
  }
  return args;
}

static bool parseLogLevel(const std::string& arg) {
  if (Log::logLevels.count(arg) == 0)
    return false;
  logger.setLogLevel(Log::logLevels.at(arg));
  return true;
}

static bool parseLogFolder(const std::string& arg) {
  if (arg.find("//") != std::string::npos)
    return false;
  const std::string normalized = arg[arg.length() - 1] == '/' ? arg.substr(0, arg.length() - 1) : arg;
  if (normalized.empty())
    return false;
  logger.setLogFolder(normalized);
  return true;
}

int main(int argc, char* argv[]) {
  --argc;
  ++argv;
  if (argc == 0)
    usage();

  std::vector<std::string> args = getCommandLineArguments(argv);
  std::string filename;

  for (std::vector<std::string>::const_iterator it = args.begin(); it != args.end(); ++it) {
    if ((*it)[0] != '-') {
      if (filename.empty())
        filename = *it;
      else
        usage("Error: multiple configuration files provided.", 1);
    } else if (*it == "--help" || *it == "-h") {
      usage();
    } else if (*it == "--log-file") {
      ++it;
      if (it == args.end())
        usage("Error: no log folder specified.", 1);
      logger.setWriteToFile(true);
      if (!parseLogFolder(*it))
        usage("Error: invalid log folder.");
    } else if (*it == "--log-level") {
      ++it;
      if (it == args.end())
        usage("Error: no log level specified.", 1);
      if (!parseLogLevel(*it))
        usage("Error: invalid log level.", 1);
    } else {
      usage("Error: invalid option: " + *it);
    }
  }
  if (filename.empty())
    usage("Error: no configuration file provided.");

  try {
    Server server(8080);
  } catch (std::exception& e) {
  }
  return 0;
}
