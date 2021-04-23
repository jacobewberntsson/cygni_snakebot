from ubuntu:latest

RUN apt-get update \
    && apt-get install -y llvm make clang

RUN useradd -ms /bin/bash snakebot
USER snakebot
