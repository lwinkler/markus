# git clone https://github.com/miloyip/rapidjson.git

all: bench_features
bench_features: bench_features.cpp
	g++  bench_features.cpp -I/usr/include/boost -Irapidjson/include -ljsoncpp -o bench_features
