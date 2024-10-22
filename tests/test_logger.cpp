#include <iostream>

#include <Log.h>

int main() {
  { // TEST: default log level
    std::cout << "==== TEST: default log level is 'info' ====\n";
    Log logger;
    logger.debug("test dbg");
    logger.info("test info");
    logger.warn("test warn");
    logger.error("test error");
  }

  std::cout << "\n================================================\n\n";

  { // TEST: debug
    std::cout << "==== TEST: debug ====\n";
    Log logger;
    logger.setLogLevel(Log::Debug);
    logger.debug("test dbg");
    logger.info("test info");
    logger.warn("test warn");
    logger.error("test error");
  }

  std::cout << "\n================================================\n\n";

  { // TEST: warn
    std::cout << "==== TEST: warn ====\n";
    Log logger;
    logger.setLogLevel(Log::Warn);
    logger.debug("test dbg");
    logger.info("test info");
    logger.warn("test warn");
    logger.error("test error");
  }

  std::cout << "\n================================================\n\n";

  { // TEST: error
    std::cout << "==== TEST: error ====\n";
    Log logger;
    logger.setLogLevel(Log::Error);
    logger.debug("test dbg");
    logger.info("test info");
    logger.warn("test warn");
    logger.error("test error");
  }

  return 0;
}
