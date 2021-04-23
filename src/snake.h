#pragma once

#include "json.hpp"
#include <unordered_map>
#include <map>
#include <string>
#include "util.h"

class Snake
{
public:
  std::string name = "SafeBot1.1";
  std::string id = "";
  std::string get_next_move(nlohmann::json map);
  void on_game_ended();
  void on_tournament_ended();
  void on_snake_dead(std::string death_reason);
  void on_game_starting();
  void on_player_registered();
  void on_invalid_playername();
  void on_game_result(nlohmann::json playerRanks);
  void make_map(int width, int height);
  void update_map(nlohmann::json map);
  bool in_grid(int, int, int, int);
  void pos2coord(int, int, int*, int*);
  int  coord2pos(int, int, int);
  int  openess(int*,int, Direction& direction, int, int, std::vector<int>,std::vector<int>, std::vector<std::vector<int>>, int);
  bool init_map = false;
  int* good_map[4];
  int ctr = 2;
  bool has_id = false;
};
