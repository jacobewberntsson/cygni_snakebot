# Snakebot Client for C++
C++, whooo!

## Editing
First, change the name of your Snakebot in `src/snake.h`. The name
of your Snakebot must be unique!

## Game modes
In `src/main.cpp` in the following code snippet:
```C
static const std::string host = "snake.cygni.se";
static const std::string port = "80";
static const std::string venue = "training";
```
you may change the venue to either 'training' or 'tournament'.

## Coding
Edit the snake at `src/snake.cpp`, primarily the `get_next_move` function.

## Building and running
Ensure you have the necessary websocket package:
```bash
# fetch the necessary external dependencies
git submodule update --init --recursive 
# build and run the snakebot
make run
# or just build it with
make build
``` 

## Docker
You can also use Docker to build and run the project:
```
docker-compose run snakebot make run
```

## Modifying the client itself
If you modify the client, run `make test` to verify that the test suite is still passing.
