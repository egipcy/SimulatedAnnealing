#pragma once

#include <random>

class RandomGenerator
{
public:
  static RandomGenerator& get_instance();

  size_t get_random(size_t max_value);

  RandomGenerator(const RandomGenerator&) = delete;
  RandomGenerator& operator=(const RandomGenerator) = delete;

private:
  static RandomGenerator instance_;

  RandomGenerator(size_t seed);
  ~RandomGenerator();

  std::default_random_engine generator_;
};
