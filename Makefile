all:
	g++ test.cpp sequential.cpp bidirectional.cpp centralized.cpp -lpthread

maze.java:
	javac MazeGenerator.java