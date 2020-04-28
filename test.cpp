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

    Map *map = new Map(mat);

//    int length = find_path_bidirectional(map);

    find_path_sequential(map);

//    std::cout << length << std::endl;
}