TBB_INCLUDE_PATH=tbb/include
TBB_LIB_PATH=tbb/lib/intel64/gcc4.8/

C_FILE=test.cpp sequential.cpp bidirectional.cpp spa.cpp hda.cpp pla.cpp


all: main

main: ${C_FILE}
	g++ ${C_FILE} -fopenmp -lpthread -ltbb -I${TBB_INCLUDE_PATH} -L${TBB_LIB_PATH} -o main

run: main
	LD_PRELOAD=${TBB_LIB_PATH}libtbb.so.2 ./main ${file} 
	

maze.java:
	javac MazeGenerator.java