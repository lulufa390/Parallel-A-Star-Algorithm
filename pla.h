//
// Created by Jikai Lu on 4/30/20.
//

#ifndef FINAL_PLA_H
#define FINAL_PLA_H

#include <omp.h>
#include <vector>
#include "map.h"

class thread_state {
public:
    priority_queue<pair<int, Node *>> open_list;

//    vector<int> g_value;
};


extern int best_soln;
extern int optimal_length;

extern thread_state* thread_array;

extern vector<int> start_g_value;



//extern Map* map;

int compute_pla(const Map* map, int thread_count);



#endif //FINAL_PLA_H
