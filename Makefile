all:
	g++ test.cpp sequential.cpp bidirectional.cpp -lpthread

maze.java:
	javac MazeGenerator.java