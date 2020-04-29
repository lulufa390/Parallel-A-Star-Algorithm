//
// Created by Jikai Lu on 4/24/20.
//
#include "map.h"
#include "sequential.h"
#include "bidirectional.h"

int main() {

    // Map *map = new Map("maze_3_3.txt");
    // find_path_sequential(map);
    // return 0;

    std::vector<std::vector<int> > mat = {
            {0, 1, 2, 0, 0, 0, 0, 2, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 2, 0, 0},
            {0, 0, 2, 0, 0, 0, 0, 2, 0, 0},
            {0, 0, 2, 0, 0, 0, 0, 2, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 2, 0, 0, 0, 0, 0, 2, 0, 0},
            {0, 2, 0, 0, 0, 0, 0, 2, 0, 3},
            {0, 2, 0, 0, 0, 0, 0, 2, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 2, 0, 0},
    };
    

    std::vector<std::vector<int>> mat2;

    int size = 1000;

    for (int i = 0; i < size; i++) {
        if (i % 2 && i < size - 1) {
            std::vector<int> line(size, 2);
            line[rand() % size] = 0;
            mat2.push_back(line);
        }
        else {
            mat2.push_back(std::vector<int>(size, 0));
        }
    }

    mat2[0][size/2] = 1;

    mat2[size - 1][size/2] = 3;

    clock_t start, stop;
    clock_t start2, stop2;

    // sequential
    // Map *map1 = new Map(mat2);
    std::string test_file_name = "maze_case/maze_1000_1000.txt";
    Map *map = new Map(test_file_name);
    start = clock();
    int length1 = find_path_sequential(map);
    stop = clock();

    // bidirectional
    start2 = clock();
    int length2 = find_path_bidirectional(map);
    stop2 = clock();


    std::cout << "Sequential Total Time : " <<(double)(stop - start) / CLOCKS_PER_SEC << "s" << std::endl;
    std::cout << "Sequential shortest path length : " << length1 << std::endl;
    std::cout << "Bidirectional Total Time : " <<(double)(stop2 - start2) / CLOCKS_PER_SEC << "s" << std::endl;
    std::cout << "Bidirectional shortest path length : " << length2 << std::endl;
}