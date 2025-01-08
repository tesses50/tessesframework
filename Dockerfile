FROM ubuntu:noble

RUN apt update && apt install -y cmake g++ gcc libmbedtls-dev build-essential git ca-certificates

RUN mkdir /src && cd /src && git clone https://onedev.site.tesses.net/tesses-framework . && mkdir build && cd build && cmake -S .. -B . && make -j12 && make install && cd / && rm -r /src
