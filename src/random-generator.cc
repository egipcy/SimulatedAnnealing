#include "random-generator.hh"

#include <ctime>
#include <chrono>

RandomGenerator RandomGenerator::instance_(std::chrono::system_clock::now().time_since_epoch().count());

size_t RandomGenerator::get_random(size_t max_value)
{
  std::uniform_int_distribution<size_t> distribution(0, max_value);
  return distribution(generator_);
}

RandomGenerator::RandomGenerator(size_t seed)
  : generator_(seed)
{ }

RandomGenerator::~RandomGenerator()
{ }

RandomGenerator& RandomGenerator::get_instance()
{
  return instance_;
}
