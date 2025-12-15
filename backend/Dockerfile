FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libboost-all-dev \
    libpqxx-dev \
    wget

RUN wget https://github.com/CrowCpp/Crow/releases/download/v1.0+5/crow_all.h -O /usr/include/crow_all.h

WORKDIR /app

COPY . .

RUN mkdir build && cd build && cmake .. && make

CMD ["./build/server"]