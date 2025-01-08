FROM ubuntu:noble

RUN apt update && apt install -y cmake g++ gcc libmbedtls-dev build-essential

RUN ls && mkdir build && cd build && cmake -S .. -B . && make -j12 && make install