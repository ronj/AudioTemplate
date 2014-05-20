#include "yaffut.h"

#include <common/logging/g2log.hpp>
#include <common/logging/g2logworker.hpp>

struct LoggingTest {};

TEST(LoggingTest, should_construct_default_logger)
{
  auto logworker = g2::LogWorker::createWithDefaultLogger("LoggingTest", ".");
  g2::initializeLogging(logworker.worker.get());
}

struct TestSink
{
public:
  TestSink(std::string& aOutput) : iOutput(aOutput) {}
  void write(g2::LogMessageMover aMessage)
  {
    iOutput = aMessage.get().toString();
  }

private:
  std::string& iOutput;
};

TEST(LoggingTest, should_construct_logger_with_custom_sink)
{
  std::string logOutput;
  {
    auto logworker = g2::LogWorker::createWithNoSink();
    auto sinkHandle = logworker->addSink(std2::make_unique<TestSink>(std::ref(logOutput)), &TestSink::write);
    g2::initializeLogging(logworker.get());

    LOG(INFO) << "Hello, Logger!";
  }

  std::size_t found = std::string::npos;
  found = logOutput.find("Hello, Logger!");
  CHECK(found != std::string::npos);

  found = logOutput.find("INFO");
  CHECK(found != std::string::npos);
}
