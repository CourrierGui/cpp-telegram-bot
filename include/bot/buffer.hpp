#pragma once

#include <yaml-cpp/yaml.h>

#include <random>
#include <algorithm>
#include <vector>

struct RandInt {
  mutable std::mt19937 seed;
  mutable std::uniform_real_distribution<double> gen;

  RandInt();
  int operator()(int min, int max) const;
};

class MessageBuffer {
  private:
    mutable std::vector<YAML::Node> messages;
    mutable RandInt rand_int;
    mutable int end;

  public:
    MessageBuffer(const YAML::Node& node);
    auto operator()(void) const -> YAML::Node;

};
