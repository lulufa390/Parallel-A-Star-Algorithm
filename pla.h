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
    priority_queue<pair<int, pair<Node*, Node *>>> open_list;
    int count;

    std::queue<pair<int, pair<Node*,Node *>>> wait_list;

    std::vector<thread_state*> neighbors;

    omp_lock_t lock;

public:
    thread_state() : count(0){
        omp_init_lock(&lock);
    }
};


extern int optimal_length;

extern std::atomic<int> open_node_num;

extern thread_state* thread_array;

extern std::vector<int> start_g_value;

//extern std::queue<pair<int, Node *>> wait_list;


TestResult* find_path_pla(const Map* map, int thread_count);


#endif //FINAL_PLA_H
