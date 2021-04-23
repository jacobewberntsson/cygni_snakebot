#include "easylogging++.h"
#include "json.hpp"
#include "snake.h"
#include <iostream>
#include <stdlib.h>
#include <queue> 
#include <stack>
#include <chrono>
#include <ctime>

#define DEBUG_TIME (0)

using nlohmann::json;

const static int STAR = 8;
const static int HOLE = 9;

Direction directions[4] = {Direction::Down,Direction::Up,Direction::Left,Direction::Right};

std::string Snake::get_next_move(json map) { 
  #if DEBUG_TIME
  auto start = std::chrono::system_clock::now();
  #endif
  int width  = map["width"];
  int height = map["height"];
  if (!init_map) {
    init_map = true;
    make_map(width, height);
  }
  update_map(map);
 
  std::vector<int> heads;
  std::vector<int> own_body;
  std::vector<std::vector<int>> bodies;
  int head;
  std::string response = "DOWN";
  int best_move = 0;

  for(auto it = map["snakeInfos"].begin(); it != map["snakeInfos"].end(); ++it){
    std::string snake_id = (*it)["id"];
    if (!has_id) {
      std::string snake_name = (*it)["name"];
      if (name.compare(snake_name) == 0) {
        id = snake_id;
        has_id;
      }
    }
    json positions = (*it)["positions"];
    if (!positions.empty()) {
      if (id.compare(snake_id) == 0) {
        head = positions[0].get<int>();
        for (auto position_it = positions.begin(); position_it != positions.end(); ++position_it) {
          int c = *position_it;
          own_body.push_back(c);
        }
      }else {
        std::vector<int> b;
        heads.push_back(positions[0]);
        for (auto position_it = positions.begin(); position_it != positions.end(); ++position_it) {
          int c = *position_it;
          b.push_back(c);
        }
        bodies.push_back(b);
      }
    }
  }
  int actions_x[4] = {-1, 1, 0, 0};
  int actions_y[4] = {0,  0, 1 ,-1};
  int x, y;
  int game_tick = map["worldTick"];
  for (int i = 0; i < 4; ++i) {
    int cnt = openess(good_map[i], head, directions[i], width, height, heads, own_body, bodies, game_tick);
    //std::cout << cnt << std::endl;
    pos2coord(head, width, &x, &y);
    x += actions_x[i];
    y += actions_y[i];
    float discount = (y == 0 || y == height-1) ? 0.8 : 1;
    cnt = discount*cnt;
    if (cnt > best_move) {
      best_move = cnt;
      response = direction_as_string(directions[i]);
    }
  }
  //std::cout << response << std::endl;
  
  #if DEBUG_TIME
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;
  std::cout << "elapsed time: " << elapsed_seconds.count() * 1000 << "s\n";
  #endif
  return response;
}
bool Snake::in_grid(int x, int y, int width, int height) {
  return y >= 0 && y < height && x >= 0 && x < width;
}
void Snake::pos2coord(int pos, int width, int* x, int* y) {
  (*y) = pos / width;
  (*x) = pos - (*y) * width;
}
int  Snake::coord2pos(int x, int y, int width) {
  return width * y + x;
}

int Snake::openess(int* map, const int pos, Direction& direction, int width, int height,
             std::vector<int> heads, std::vector<int> body, std::vector<std::vector<int>> bodies, int tick) {
  int d = 0;
  int cnt = 0;
  int p;
  int y;
  int x;
  int game_tick = tick;
  std::shared_ptr<std::queue<int>> old_q(new std::queue<int>());
  std::shared_ptr<std::queue<int>> new_q(new std::queue<int>());
  std::shared_ptr<std::queue<int>> other_old(new std::queue<int>());
  std::shared_ptr<std::queue<int>> other_new(new std::queue<int>());
  std::shared_ptr<std::queue<int>> temp1;
  std::shared_ptr<std::queue<int>> temp2;
  
  std::vector<std::stack<int>> other_stack;
  std::stack<int> self_stack;
  //std::cout << "-------------------------------------------" << std::endl;
  for(auto it = bodies.begin(); it != bodies.end(); ++it) {
    auto l = *(it);
    std::stack<int> s;
    for (auto it_l = l.begin(); it_l != l.end(); ++it_l ) {
      s.push((*it_l));
    }
    other_stack.push_back(s);
  }

  for(auto it = body.begin() ; it != body.end(); ++it) {
    self_stack.push((*it));
  }

  int actions_x[4] = {-1, 1, 0, 0};
  int actions_y[4] = {0,  0, 1 ,-1};

  
  for(auto it = heads.begin(); it != heads.end(); ++it) {
    p = (*it);
    pos2coord(p, width, &x, &y);
    //std::cout << y << ":" << x << std::endl;
    for (int i = 0; i < 4; ++i) {
      int rx = x + actions_x[i];
      int ry = y + actions_y[i];
      if (in_grid(rx, ry, width, height)) {
        p = coord2pos(rx, ry, width);
        if (map[p] == 0 || map[p] == STAR) {
          map[p] = 3;
          other_old->push(p);
        }
      }
    }
  }


  //std::cout << other_old->size() << std::endl;

  pos2coord(pos, width, &x, &y);

  switch(direction) {
  case Direction::Down:
    y += 1;
    break;
  case Direction::Up:
    y -= 1;
    break;
  case Direction::Left:
    x -= 1;
    break;
  case Direction::Right:
    x += 1;
    break;
  }


  if (in_grid(x, y, width, height)) {
    p = coord2pos(x, y, width);
    if (map[p] == 0 || map[p] == STAR) {
      map[p] = 4;
      cnt++;
      old_q->push(p);
    }else {
      return 0;
    }
  }else {
    return 0;
  }
  if (game_tick % 3 != 0) {
    for(auto it = other_stack.begin(); it != other_stack.end(); ++it) {
      if (!it->empty()) {
        p = it->top();
        it->pop();
        map[p] = 0;
      }    
    }
    if (!self_stack.empty()) {
      p = self_stack.top();
      self_stack.pop();
      map[p] = 0;
    }
  }
  game_tick++;
  while (!old_q->empty()) {
    while(!other_old->empty()) {
      p = other_old->front();
      other_old->pop();
      pos2coord(p, width, &x, &y);

      for (int i = 0; i < 4; ++i) {
        int rx = x + actions_x[i];
        int ry = y + actions_y[i];
        if (in_grid(rx, ry, width, height)) {
          p = coord2pos(rx, ry, width);
          if (map[p] == 0 || map[p] == STAR) {
            map[p] = 7;
            other_new->push(p);
            d++;
          }
        }
      }
    }
    while(!old_q->empty()) {
      p = old_q->front();
      pos2coord(p, width, &x, &y);
      for (int i = 0; i < 4; ++i) {
        int rx = x + actions_x[i];
        int ry = y + actions_y[i];
        if (in_grid(rx, ry, width, height)) {
          p = coord2pos(rx, ry, width);
          if (map[p] == 0 || map[p] == STAR) {
            map[p] = 5;
            cnt++;
            new_q->push(p);
          }
        }
      }
      old_q->pop();

    }
    if (game_tick % 3 != 0) {
      for(auto it = other_stack.begin(); it != other_stack.end(); ++it) {
        if (!it->empty()) {
          p = it->top();
          it->pop();
          map[p] = 0;
          
        }    
      }
      if (!self_stack.empty()) {
        p = self_stack.top();
        self_stack.pop();
        //std::cout << p << ":" << pos << std::endl;
        map[p] = 0;

        //std::cout << p << std::endl;
      }
    }
    
    game_tick++;
    temp1 = other_old;
    other_old = other_new;
    other_new = temp1;
    
    temp2 = old_q;
    old_q = new_q;
    new_q = temp2;
  }
  

  return cnt;
}

void Snake::on_game_ended() {
  LOG(INFO) << "Game has ended";
};

void Snake::on_tournament_ended() {
  LOG(INFO) << "Tournament has ended";
};

void Snake::on_snake_dead(std::string death_reason) {
  LOG(INFO) << "Our snake has died, reason was: " << death_reason;
};

void Snake::on_game_starting() {
  LOG(INFO) << "Game is starting";
  
};

void Snake::on_player_registered() {
  LOG(INFO) << "Player was successfully registered";
};

void Snake::on_invalid_playername() {
  LOG(INFO) << "The player name is invalid, try another?";
};

void Snake::on_game_result(nlohmann::json playerRanks) {
  LOG(INFO) << "Game result:";
  nlohmann::json playerRank;
  el::Logger* defaultLogger = el::Loggers::getLogger("default");
  for (json::iterator it = playerRanks.begin(); it != playerRanks.end(); ++it) {
    playerRank = (nlohmann::json) *it;
    defaultLogger->info("%v.\t%v pts\t%v (%v)", playerRank["rank"], playerRank["points"],
            playerRank["playerName"], playerRank["alive"] ? "alive" : "dead");
  }
}

void Snake::make_map(int width, int height) {
  for (int i = 0; i < 4; ++i) {
    good_map[i] = new int[height * width];
  }
}
void Snake::update_map(json map) {
  int width  = map["width"];
  int height = map["height"];
  for(int i = 0; i < 4 ; ++i) {
    memset(good_map[i], 0, sizeof(int) * width * height);
  }
  for(auto it = map["snakeInfos"].begin(); it != map["snakeInfos"].end(); ++it){
    std::string snake_id = (*it)["id"];
    int val = 2;    
    if (id.compare(snake_id) == 0) {
      val = 1;
    }
    json positions = (*it)["positions"];

    for (auto position_it = positions.begin(); position_it != positions.end(); ++position_it) {
      int c = position_it->get<int>();
      for (int i = 0; i < 4; ++i) {
        good_map[i][c] = val;
      }
    }
  }

  for(auto it = map["foodPositions"].begin(); it != map["foodPositions"].end(); ++it) {
    int c = it->get<int>();
    for (int i = 0; i < 4; ++i) {
      good_map[i][c] = STAR;
    }
  }
  for(auto it = map["obstaclePositions"].begin(); it != map["obstaclePositions"].end(); ++it) {
    int c = it->get<int>();
    for (int i = 0; i < 4; ++i) {
      good_map[i][c] = HOLE;
    }
  }

  /*for(int y = 0; y < height; ++y){
    for(int x = 0; x < width; ++x){
      std::cout<<good_map[y*width + x];
    }
    std::cout << std::endl;
  }
  std::cout << "----------------------------------" << std::endl;*/
}
