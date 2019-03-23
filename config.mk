CXXFLAGS:=-O3 -Wall -std=c++17 -flto -g -march=native -I. -fno-exceptions \
	$(shell pkg-config --cflags gflags)
LDFLAGS:=-flto -lpthread $(shell pkg-config --libs gflags)
CXX:=g++
