//
// Created by Jikai Lu on 4/30/20.
//

#ifndef FINAL_PLA_H
#define FINAL_PLA_H

#include <omp.h>
#include <vector>
#include <queue>
#include <atomic>
#include "map.h"

class thread_state {
public:
    priority_queue<pair<int, Node *>> open_list;
    int count;

public:
    thread_state() : count(0){}
};


extern int optimal_length;

extern std::atomic<int> open_node_num;

extern thread_state* thread_array;

extern std::vector<int> start_g_value;

extern std::queue<pair<int, Node *>> wait_list;


int find_path_pla(const Map* map, int thread_count);


#endif //FINAL_PLA_H
