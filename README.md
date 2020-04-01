# Distributed Map/Reduce framework
It is framework, which you can easily use for Map/Reduce model. As C++ is compiled language, so it is not easy to spread code 
among multiple computers. It is done by the use of shared library and polymorphism, so user need just to implement map, reduce 
functions and serialization of used types (all primitive types are available). Moreover, on each computer data is processed
concurrently.


## Table of contents
 - [Dependencies](#requirements)
 - [Installing](#installing)
 - [Usage](#usage)
 - [Contributors](#contributors)

## Dependencies

For using this utility you need to have next third-pary libraries
- Libssh
- Boost (System, Filesystem, Program options)

In ubuntu you can easily intall them using next command:
```sh
sudo apt install libssh-dev libboost-all-dev
```

Also, Dockerfile is provided with preinstalled libraries. You can build it with command:
```sh
docker build . -t milishchuk/mapreduce
```


## Installing
```sh
mkdir build
cd build
cmake ..
make
make install
```
## Usage
This repo contains [example](https://github.com/RomanMilishchuk/distributed_map_reduce/tree/master/example), which you can use
for better understanding. We can divide implementing by next phases:

### Custom KeyValueType
If you need specific type for your data, you should implement `KeyValueType` interface with `void parse(const std::string&)`
and `std::string to_string() const` methods. Or you can use implemented primitives(char, int, double, long). Implement
custom `KeyValueTypeFactory` for this type.

### Map/Reduce
Implement `Map` and `Reduce` classes inherited by `map_base` and `reduce_base` interfaces.

### JobConfig
Implement function `std::shared_ptr<job_config> get_config()`, which will return config with `map`,`reduce` functions, and
Factories for `key_in`, `key_out`, `value_in`, `value_out`, `value_res`. For better understanding next diagram:
<pre>
                map                     groupby                      reduce
key_in, key_out ==> key_out, value_out  
key_in, key_out ==> key_out, value_out  
key_in, key_out ==> key_out, value_out  ======> key_out, [value_out] =====> key_out, value_res
key_in, key_out ==> key_out, value_out 
key_in, key_out ==> key_out, value_out  
</pre>

### Shared library
Nextly, you need to make shared library with `std::shared_ptr<job_config> get_config()` function

### Run
You can use blocking `run_task_blocking` function or non-blocking `run_task`, which will return `std::future`. All data
to map nodes are read from files on corresponding computers.

### Preview
For quick enviroment setup you can use [run_example.sh](https://github.com/RomanMilishchuk/distributed_map_reduce/blob/master/run_example.sh)
script, which launchs 4 map nodes, reduce node and master node to which result will be returned.

## Contributors

Roman Milishchuk [@RomanMilishchuk](https://github.com/RomanMilishchuk)
