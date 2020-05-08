This project implements several parallel A* algorithm, including PNBA*, SPA*, PLA* and HDA*.

# Installation and Compilation
<pre>
To compile the test program, g++ and Intel Thead Building Block(tbb) needs to be installed. 

To install g++, run: 
    sudo apt update
    sudo apt install g++

To install tbb, visit https://software.seek.intel.com/performance-libraries, and download the compressed file.
Decompress it, enter the folder and run bash.sh to install tbb library.
At the root folder of this repo, make a soft link named tbb to the tbb folder of the installed library. (Should be at the $HOME path). Run:
    ln -s $HOME/intel/tbb tbb

To compile the test program, run:
    make

To run the test program, run:
    make run file=maze_case/&ltmaze_case_file&gt

Several example maze files are provided in the maze_case folder.
</pre>
# Maze Generator
<pre>

To compile the maze generator, run:
    make maze.java

To generate the maze, run:
    java MazeGenerator &ltmaze_width&gt &ltmaze_height&gt &ltoutput_file&gt

Our maze generator are modified based on the maze generator from the following link:
    https://www.rosettacode.org/wiki/Maze_generation#C.2B.2B
</pre>


# Implemented Algorithms.
<pre>
We implement 4 parallel A* algorithms.
PNBA* is a bidirectional algorithms. It is run only with 2 threads.
SPA*, PLA* and HDA* can be run on multiple threads. In our test program, it is run with 1, 2, 4, 8, 16 and 32 threads.
Details of these algorithms can be found on the papers in the paper folder
</pre>
