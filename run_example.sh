#cd cmake-build-debug && cmake .. && make -j4 && make install && cd -
docker kill $(docker ps -q)
for i in 1 2 3 4 5; do docker run -v /home/midren/ucu/distributed_db/distributed_map_reduce/:/home/mapreduce/distributed_map_reduce -itd milishchuk/mapreduce:latest /bin/bash; done
echo "cd distributed_map_reduce  && rm -rf build && mkdir build && cd build && cmake .. && make -j4 && make install && cd ../example/ && rm -rf build && mkdir build && cd build &&cmake .. && make -j4 && export LD_LIBRARY_PATH=/home/mapreduce/distributed_map_reduce/example/build && ./example" | docker run -v /home/midren/ucu/distributed_db/distributed_map_reduce/:/home/mapreduce/distributed_map_reduce -a stdin -a stdout -a stderr -i milishchuk/mapreduce:latest
