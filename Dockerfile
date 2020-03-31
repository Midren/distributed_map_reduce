FROM ubuntu:19.04

RUN apt-get update -y && \
    apt-get install -y \
    g++ \
    cmake \
    libssh-dev \
    libboost-all-dev \
    git \
    make

# For reduce node
EXPOSE 8001
# For master node
EXPOSE 8002

RUN useradd -m mapreduce
USER mapreduce

WORKDIR /home/mapreduce
RUN git clone https://github.com/RomanMilishchuk/DistributedMapReduce distributed_map_reduce && \
    cd distributed_map_reduce && \
    mkdir build && \
    cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make && \
    make install

ENV PATH="${PATH}:/home/mapreduce/distributed_map_reduce/bin"
