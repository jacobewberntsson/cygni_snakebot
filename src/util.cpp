#include <string>
#include <math.h>
#include "optional.hpp"
#include "util.h"
#include "structs.h"
#include <algorithm>

using std::experimental::optional;

std::string direction_as_string(const Direction& direction) {
  switch(direction) {
  case Direction::Down:
    return "DOWN";
  case Direction::Up:
    return "UP";
  case Direction::Left:
    return "LEFT";
  case Direction::Right:
    return "RIGHT";
  }
}

std::tuple<int, int> direction_as_movement_delta(const Direction& direction) {
  switch(direction) {
  case Direction::Down:
    return std::make_tuple(0, 1);
  case Direction::Up:
    return std::make_tuple(0, -1);
  case Direction::Left:
    return std::make_tuple(-1, 0);
  case Direction::Right:
    return std::make_tuple(1, 0);
  }
}

bool inside_map(const Map& map, const std::tuple<int, int>& coordinate) {
  int x, y;
  std::tie(x, y) = coordinate;
  bool inside_x = x >= 0 && x < map.width;
  bool inside_y = y >= 0 && y < map.height;
  return inside_x && inside_y;
}

optional<Snake_t> get_snake_by_id(const Map& map, const std::string& id) {
  auto it =
    std::find_if(map.snakeInfos.begin(),
                 map.snakeInfos.end(),
                 [id](Snake_t snake) {
                   return snake.id == id;
                 });

  if (it != map.snakeInfos.end()) {
    return *it;
  }

  return std::experimental::nullopt;
}

bool list_contains_position(const std::vector<int>& positions, const int position) {
  return std::find(positions.begin(),
                   positions.end(),
                   position) != positions.end();
}

Tile get_tile_at(const Map& map, const std::tuple<int, int>& coordinate) {
  int position = translate_coordinate(coordinate, map.width);
  auto it = std::find_if(map.snakeInfos.begin(),
                         map.snakeInfos.end(),
                         [position](Snake_t snake) {
                           return list_contains_position(snake.positions, position);
                         });

  if (list_contains_position(map.obstaclePositions, position)) {
    return Tile::Obstacle;
  } else if (list_contains_position(map.foodPositions, position)) {
    return Tile::Food;
  } else if (it != map.snakeInfos.end()) {
    Snake_t snake = *it;

    if (snake.positions[0] == position) {
      return Tile::SnakeHead;
    } else if (snake.positions[snake.positions.size() - 1] == position) {
      return Tile::SnakeTail;
    } else {
      return Tile::SnakeBody;
    }
  } else if (!inside_map(map, coordinate)) {
    return Tile::Wall;
  } else {
    return Tile::Empty;
  }
}

bool is_tile_available_for_movement(const Map& map, const std::tuple<int, int>& coordinate) {
  Tile tile = get_tile_at(map, coordinate);
  switch(tile) {
  case Tile::Empty:
  case Tile::Food:
    return true;
  default:
    return false;
  }
}

bool can_snake_move_in_direction(const Map& map, const Snake_t& snake, const Direction& direction) {
  int x, y, xd, yd;

  std::tie(xd, yd) = direction_as_movement_delta(direction);
  std::tie(x, y) = translate_position(snake.positions[0], map.width);

  std::tuple<int, int> new_coord = std::make_tuple(x+xd, y+yd);

  return is_tile_available_for_movement(map, new_coord);
}

bool is_coordinate_out_of_bounds(const Map& map, const std::tuple<int, int>& coordinate) {
  int x, y;
  std::tie(x,y) = coordinate;
  return x < 0 || x >= map.width || y < 0 || y >= map.height;
}

std::tuple<int, int> translate_position(const int position, const int map_width) {
  float pos = position;
  float width = map_width;

  int y = floor(pos / width);
  int x = fabs(pos - y * width);

  return std::make_tuple(x, y);
}

std::vector<std::tuple<int, int>> translate_positions(const std::vector<int>& positions, const int map_width) {
  std::vector<std::tuple<int, int>> result;
  result.resize(positions.size());

  std::transform(positions.begin(), positions.end(), result.begin(),
                 [map_width](int pos) {
                   return translate_position(pos, map_width);
                 });

  return result;
}

int translate_coordinate(const std::tuple<int, int>& coordinates, const int map_width) {
  int x, y;
  std::tie(x, y) = coordinates;
  return x + y * map_width;
}

int get_manhattan_distance(const std::tuple<int, int>& start, const std::tuple<int, int>& goal) {
  int x1, y1, x2, y2;

  std::tie(x1, y1) = start;
  std::tie(x2, y2) = goal;

  int x = abs( x1 - x2 );
  int y = abs( y1 - y2 );

  return x+y;
}

float get_euclidian_distance(const std::tuple<int, int>& start, const std::tuple<int, int>& goal) {
  int x1, y1, x2, y2;

  std::tie(x1,y1) = start;
  std::tie(x2,y2) = goal;

  int x = pow(x1 - x2, 2);
  int y = pow(y1 - y2, 2);
  float d = (x + y);

  return floor(sqrt(d));
}

bool is_within_square(const std::tuple<int, int>& coord,
                      const std::tuple<int, int>& ne_coord,
                      const std::tuple<int, int>& sw_coord) {
  int x, y, ne_x, ne_y, sw_x, sw_y;

  std::tie(x,y) = coord;
  std::tie(ne_x, ne_y) = ne_coord;
  std::tie(sw_x, sw_y) = sw_coord;

  return x < ne_x || x > sw_x || y < sw_y || y > ne_y;
}
