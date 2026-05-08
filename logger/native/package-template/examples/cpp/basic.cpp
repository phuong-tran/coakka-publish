#include "coakka/logger/native_cpp/Connector.h"

#include <chrono>
#include <iostream>
#include <stdexcept>

int main() {
  using coakka::logger::native_cpp::LoggerOrchestrator;
  using coakka::logger::native_cpp::StartSpec;

  StartSpec spec;
  spec.system_name = "logger-cpp-basic";
  spec.queue_capacity = 8;
  spec.category_capacity = 64;
  spec.message_capacity = 128;

  LoggerOrchestrator logger(spec);
  const auto sequence = logger.logInfo("startup", "logger core booted");
  const auto record = logger.await(std::chrono::milliseconds(100));
  if (!record.has_value()) {
    throw std::runtime_error("expected one logger record");
  }

  std::cout << "sequence=" << sequence
            << " level=" << record->level_name
            << " category=" << record->category
            << " message=" << record->message
            << '\n';

  logger.stop();
  return 0;
}
