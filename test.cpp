//
// Created by Jikai Lu on 4/24/20.
//
#include "map.h"
#include "sequential.h"
#include "bidirectional.h"

int main() {

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

    int size = 300;

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

    Map *map = new Map(mat2);


    clock_t start, stop;
    clock_t start2, stop2;
    start = clock();

//    int length = find_path_bidirectional(map);
    int length1 = find_path_sequential(map);
    stop = clock();


    Map *map22 = new Map(mat2);
    start2 = clock();

    int length2 = find_path_bidirectional(map22);
    stop2 = clock();
//    std::cout << length1 << std::endl;
//    std::cout << length2 << std::endl;


    std::cout << "Sequential Total Time : " <<(double)(stop - start) / CLOCKS_PER_SEC << "s" << std::endl;
    std::cout << "Sequential shortest path length : " << length1 << std::endl;
    std::cout << "Bidirectional Total Time : " <<(double)(stop2 - start2) / CLOCKS_PER_SEC << "s" << std::endl;
    std::cout << "Bidirectional shortest path length : " << length2 << std::endl;
}