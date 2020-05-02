TBB_INCLUDE_PATH=tbb/include
TBB_LIB_PATH=tbb/lib/intel64/gcc4.8/


all:
	g++ test.cpp sequential.cpp bidirectional.cpp centralized.cpp distributed.cpp -fopenmp -lpthread -ltbb -I${TBB_INCLUDE_PATH} -L${TBB_LIB_PATH} -o main

distributed.o: distributed.cpp
	g++ -Itbb/include -Ltbb/lib/intel64/gcc4.8/  -fopenmp -ltbb -c distributed.cpp 

maze.java:
	javac MazeGenerator.java