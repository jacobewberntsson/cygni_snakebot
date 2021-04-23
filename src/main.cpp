#include "easywsclient/easywsclient.cpp"
#include "easylogging++.h"
#include "snake.h"
#include "messages.h"

#include <assert.h>
#include <stdio.h>
#include <string>
#include <memory>
#include <thread>

INITIALIZE_EASYLOGGINGPP

#ifdef _WIN32
#pragma comment( lib, "ws2_32" )
#include <WinSock2.h>
#endif

using easywsclient::WebSocket;
using nlohmann::json;

static const std::string host = "localhost";
static const std::string port = "8080";
static const std::string venue = "training";
static Snake snake;
static std::thread heart_beat_thread;

std::shared_ptr<WebSocket> connect_to_server() {
  std::string url = "ws://" + host + ":" + port + "/" + venue;
  std::shared_ptr<WebSocket> ws(WebSocket::from_url(url));

  assert(ws);
  return ws;
}

void send_heart_beat(std::shared_ptr<WebSocket> wsp, std::string id) {
  auto period = std::chrono::seconds(2);

  while (wsp->getReadyState() != WebSocket::CLOSED) {
    json heart_beat_request = heart_beat(id);
    wsp->send(heart_beat_request.dump());
    std::this_thread::sleep_for(period);
  }
}

void start_heart_beat(std::shared_ptr<WebSocket> &wsp, std::string &id) {
  heart_beat_thread = std::thread(send_heart_beat, wsp, id);
}

void route_message(std::shared_ptr<WebSocket> &wsp, const std::string & message) {
  LOG(DEBUG) << "Received message (in unparsed state)" + message;

  json incoming_json = json::parse(message.c_str());
  std::string type = incoming_json["type"];
  LOG(DEBUG) << "Received message of type " + type;
  LOG(DEBUG) << incoming_json.dump(2);

  if (type == GAME_ENDED) {
    snake.on_game_ended();
    if (venue.compare("training") == 0) {
      wsp->close();
    }
  } else if (type == TOURNAMENT_ENDED) {
    snake.on_tournament_ended();
    wsp->close();
  } else if (type == MAP_UPDATE) {
    json register_move_msg =
      register_move(snake.get_next_move(incoming_json["map"]), incoming_json);

    LOG(DEBUG) << "Responding to map update";
    LOG(DEBUG) << register_move_msg.dump(2);

    wsp->send(register_move_msg.dump());
  } else if (type == SNAKE_DEAD) {
    snake.on_snake_dead(incoming_json["deathReason"]);
  } else if (type == GAME_STARTING) {
    snake.on_game_starting();
  } else if (type == PLAYER_REGISTERED) {
    snake.on_player_registered();

    std::string game_mode = incoming_json["gameMode"];
    if (game_mode == "TRAINING") {
      LOG(DEBUG) << "Requesting a game start";

      json start_game_msg = start_game();
      wsp->send(start_game_msg.dump());
    }

    std::string id = incoming_json["receivingPlayerId"];
    snake.id = id;
    start_heart_beat(wsp, id);
  } else if (type == INVALID_PLAYER_NAME) {
    snake.on_invalid_playername();
  } else if (type == HEART_BEAT_RESPONSE) {
    // all good, do nothing
  } else if (type == GAME_LINK_EVENT) {
    std::string url = incoming_json["url"];
    LOG(INFO) << "Watch game at: " + url;
  } else if (type == GAME_RESULT_EVENT) {
    snake.on_game_result(incoming_json["playerRanks"]);
  } else {
    LOG(WARNING) << "Unable to route message, did not match any known type";
    LOG(WARNING) << incoming_json.dump(2);
  }
}

int main(int argc, char* argv[]) {
  START_EASYLOGGINGPP(argc, argv);
  el::Configurations conf("logger.conf");
  el::Loggers::reconfigureAllLoggers(conf);

  json client_info_msg = client_info();
  json player_registration_msg = player_registration(snake.name);

  auto ws = connect_to_server();
  ws->send(client_info_msg.dump());
  ws->send(player_registration_msg.dump());

  ws->poll();

  while (ws->getReadyState() != WebSocket::CLOSED) {
    ws->poll();
    ws->dispatch([&ws](const std::string & message) {
        route_message(ws, message);
      });
  }
  heart_beat_thread.join();

  LOG(INFO) << "Websocket closed, shutting down";
  return 0;
}
