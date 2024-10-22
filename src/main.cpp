#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "utils/Log.h"

static void usage(const std::string& msg = "", int exitStatus = 0) {
  if (!msg.empty())
    std::cout << msg << "\n\n";

  std::cout << "usage: webserv [OPTIONS] CONFIGURATION_FILE\n\n"
      << "Options:\n"
      << "  -h, --help             Print this help text and exit.\n\n"
      << "  --log-file             Write logs into a log file.\n"
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
      logger.setWriteToFile();
    } else if (*it == "--log-level") {
      ++it;
      if (it == args.end())
        usage("Error: no log level specified.", 1);
      if (!parseLogLevel(*it))
        usage("Error: invalid log level.", 1);
    }
  }

  return 0;
}
