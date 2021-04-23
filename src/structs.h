#pragma once

#include <vector>
#include <string>

struct Snake_t {
  std::string id;
  std::string name;
  int points;
  std::vector<int> positions;
  int tailProtectedForGameTicks;
};

struct Map {
  int height;
  int width;
  std::vector<int> foodPositions;
  std::vector<int> obstaclePositions;
  std::vector<Snake_t> snakeInfos;
  int worldTick;
};
