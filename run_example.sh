docker kill $(docker ps -q)
for i in 1 2 3 4 5; do docker run -itd milishchuk/mapreduce:latest /bin/bash; done
echo "cd distributed_map_reduce/example/ && mkdir build && cd build && cmake .. && make -j4 && export LD_LIBRARY_PATH=/home/mapreduce/distributed_map_reduce/example/build && ./example" | docker run -a stdin -a stdout -a stderr -i milishchuk/mapreduce:latest
