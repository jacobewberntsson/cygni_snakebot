#pragma once

#include <string>
#include "structs.h"
#include "optional.hpp"

enum class Tile {
    Wall,
    Food,
    Obstacle,
    Empty,
    SnakeHead,
    SnakeBody,
    SnakeTail
};

enum Direction {
    Down = 1,
    Up = 2,
    Left = 3,
    Right = 4
};

std::string direction_as_string(const Direction& direction);
std::tuple<int, int> direction_as_movement_delta(const Direction& direction);
bool inside_map(const Map& map, const std::tuple<int, int>& coordinate);
std::experimental::optional<Snake_t>get_snake_by_id(const Map& map, const std::string& id);
Tile get_tile_at(const Map& map, const std::tuple<int, int>& coordinate);
bool is_tile_available_for_movement(const Map& map, const std::tuple<int, int>&);
bool can_snake_move_in_direction(const Map& map, const Snake_t& snake, const Direction& direction);
bool is_coordinate_out_of_bounds(const Map& map, const std::tuple<int, int>&);

std::tuple<int, int> translate_position(int position, int map_width);
std::vector<std::tuple<int, int>> translate_positions(const std::vector<int>& positions, int map_width);
int translate_coordinate(const std::tuple<int, int>& coordinates, int map_width);
int get_manhattan_distance(const std::tuple<int, int>& start, const std::tuple<int, int>& goal);
float get_euclidian_distance(const std::tuple<int, int>& start, const std::tuple<int, int>& goal);
bool is_within_square(const std::tuple<int, int>& coord, const std::tuple<int, int>& ne_coord, const std::tuple<int, int>& sw_coord);
