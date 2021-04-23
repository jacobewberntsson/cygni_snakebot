#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "../src/structs.h"
#include "../src/util.h"

const int MAP_WIDTH = 3;

// The map used for testing, 1 and 2 represents the snakes
//yx012
//0  F
//1 11#
//2  2

Snake_t get_snake_one() {
  Snake_t snake;
  snake.name = "1";
  snake.points = 0;
  snake.tailProtectedForGameTicks = 0;
  snake.id = "1";

  auto p1 = std::make_tuple(1, 1);
  auto p2 = std::make_tuple(0, 1);
  snake.positions.push_back(translate_coordinate(p1, MAP_WIDTH));
  snake.positions.push_back(translate_coordinate(p2, MAP_WIDTH));

  return snake;
}

Snake_t get_snake_two() {
  Snake_t snake;
  snake.name = "2";
  snake.points = 0;
  snake.tailProtectedForGameTicks = 0;
  snake.id = "2";

  auto p1 = std::make_tuple(1, 2);
  snake.positions.push_back(translate_coordinate(p1, MAP_WIDTH));

  return snake;
}

Map construct_map() {
  Map map;
  map.width = MAP_WIDTH;
  map.height = MAP_WIDTH;
  map.worldTick = 0;
  map.snakeInfos.push_back(get_snake_one());
  map.snakeInfos.push_back(get_snake_two());

  auto foodPosition = std::make_tuple(1, 0);
  auto obstaclePosition = std::make_tuple(2, 1);

  map.obstaclePositions.push_back(translate_coordinate(obstaclePosition, MAP_WIDTH));
  map.foodPositions.push_back(translate_coordinate(foodPosition, MAP_WIDTH));

  return map;
}

TEST_CASE("snake_can_be_found_by_id", "[util]") {
  auto map = construct_map();
  auto snake_one = get_snake_one();
  auto snake = get_snake_by_id(map, snake_one.id);
  REQUIRE(snake);
};

TEST_CASE("translate_positions works", "[util]") {
  auto p1 = std::make_tuple(1, 0);
  auto p2 = std::make_tuple(1, 2);
  auto p3 = std::make_tuple(0, 1);

  REQUIRE(translate_coordinate(p1, 10) == 1);
  REQUIRE(translate_coordinate(p2, 1) == 3);
  REQUIRE(translate_coordinate(p2, 10) == 21);
  REQUIRE(translate_coordinate(p3, 3) == 3);
}

TEST_CASE("tile_is_correctly_found", "[util]") {
  std::vector<Tile> row1, row2, row3;
  std::vector<std::vector<Tile>> tiles;

  row1.push_back(Tile::Empty);
  row1.push_back(Tile::Food);
  row1.push_back(Tile::Empty);

  row2.push_back(Tile::SnakeTail);
  row2.push_back(Tile::SnakeHead);
  row2.push_back(Tile::Obstacle);

  row3.push_back(Tile::Empty);
  row3.push_back(Tile::SnakeHead);
  row3.push_back(Tile::Empty);

  tiles.push_back(row1);
  tiles.push_back(row2);
  tiles.push_back(row3);

  Map map = construct_map();

  for (int y = 0; y < map.height; y++) {
    for (int x = 0; x < map.width; x++) {
      REQUIRE(tiles[y][x] == get_tile_at(map, std::make_tuple(x, y)));
    }
  }
};

TEST_CASE("tile_is_correctly_marked_as_movable", "[util]") {
  std::vector<bool> row1, row2, row3;
  std::vector<std::vector<bool>> tiles;

  row1.push_back(true);
  row1.push_back(true);
  row1.push_back(true);

  row2.push_back(false);
  row2.push_back(false);
  row2.push_back(false);

  row3.push_back(true);
  row3.push_back(false);
  row3.push_back(true);

  tiles.push_back(row1);
  tiles.push_back(row2);
  tiles.push_back(row3);

  Map map = construct_map();
  for (int y = 0; y < map.height; ++y) {
    for (int x = 0; x < map.width; ++x) {
      REQUIRE(tiles[y][x] == is_tile_available_for_movement(map, std::make_tuple(x,y)));
    }
  }
};

TEST_CASE("can_snake_move_identifies_correctly", "[util]") {
  Map map = construct_map();
  std::string id = get_snake_one().id;
  Snake_t snake = get_snake_by_id(map, id).value();

  REQUIRE(can_snake_move_in_direction(map, snake, Direction::Up));
  REQUIRE(!can_snake_move_in_direction(map, snake, Direction::Down));
  REQUIRE(!can_snake_move_in_direction(map, snake, Direction::Left));
  REQUIRE(!can_snake_move_in_direction(map, snake, Direction::Right));
};

TEST_CASE("can_not_move_to_walls", "[util]") {
  Map map = construct_map();
  std::string id = get_snake_two().id;
  Snake_t snake = get_snake_by_id(map, id).value();

  auto can_move_down = can_snake_move_in_direction(map, snake, Direction::Down);
  REQUIRE(!can_move_down);
};
