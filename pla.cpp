//
// Created by Jikai Lu on 4/30/20.
//

#include "pla.h"

int optimal_length;
std::atomic<int> open_node_num;
thread_state* thread_array;
std::vector<int> start_g_value;
//std::queue<pair<int, Node *>> wait_list;

std::vector<pair<int, Node*> > start_expand(const Map* map, int node_number) {

    priority_queue<pair<int, Node *>> active_nodes;

    active_nodes.push({map->goal->compute_heuristic(map->start) ,map->start});

    start_g_value = vector<int>(map->height*map->width, INT32_MAX);

    start_g_value[map->start->node_id] = 0;

    std::vector<pair<int, Node*> > expand_list;

    while (!active_nodes.empty()) {

        Node *current_node = active_nodes.top().second;
        active_nodes.pop();

        if (current_node->node_id == map->goal->node_id) {
            optimal_length = start_g_value[current_node->node_id];
            return std::vector<pair<int, Node*> >();
        }

        for (auto edge : current_node->adjacent_list)
        {
            Node *node = edge.first;
            int weight = edge.second;

            int update_g_value = weight + start_g_value[current_node->node_id];

            if (update_g_value < start_g_value[node->node_id]) {
                start_g_value[node->node_id] = update_g_value;
                int new_f = update_g_value + map->goal->compute_heuristic(node);

                active_nodes.push({new_f, node});

                if (active_nodes.size() == node_number) {
                    while (!active_nodes.empty()) {
                        expand_list.push_back(active_nodes.top());
                        active_nodes.pop();
                    }
                    return expand_list;
                }
            }
        }
    }

    while (!active_nodes.empty()) {
        expand_list.push_back(active_nodes.top());
        active_nodes.pop();
    }
    return expand_list;
}


void distribute_node(std::vector<pair<int, Node*> > node_list, int thread_count) {
    srand(time(NULL));
    for (int i = 0; i < node_list.size(); i++) {
        int thread_id = rand() % thread_count;
        thread_array[thread_id].open_list.push({node_list[i].first, {NULL, node_list[i].second}});
    }

    open_node_num = node_list.size();
}

void build_hypercube(int thread_count) {

    int bit_num = (int)log2(thread_count);

    for (int i = 0; i < thread_count; i++) {
        for (int j = 0; j < bit_num; j++) {
            int neighbor = i ^ (1 << j);
            thread_array[i].neighbors.push_back(&thread_array[neighbor]);
        }
//        thread_array[i].neighbors.push_back(&thread_array[i]);
    }
}


TestResult* find_path_pla(const Map* map, int thread_count) {

    TestResult* test_result = new TestResult(thread_count);

    auto node_list = start_expand(map, thread_count);

    const int busy_threshold = 100;

    if (node_list.empty()) {
        std::cout << "do not need multi threads, the map is too small" << std::endl;
        test_result->shortest = optimal_length;
        return test_result;
    }
    else {
        thread_array = new thread_state[thread_count];
        distribute_node(node_list, thread_count);

        build_hypercube(thread_count);
        omp_set_num_threads(thread_count);
    }

    optimal_length = INT32_MAX;


#pragma omp parallel default(shared)
    {
        vector<int> g_value(start_g_value);

        int id = omp_get_thread_num();

        const int DELTA_NODE_FLUSH_PERIOD = 10;

        int delta_node_flush_count = 0;
        int delta_node_num = 0;


        while (open_node_num > 0) {

            delta_node_flush_count ++;
            if (delta_node_flush_count > DELTA_NODE_FLUSH_PERIOD) {
                delta_node_flush_count = 0;
                open_node_num += delta_node_num;
                delta_node_num = 0;
            }

            omp_set_lock(&thread_array[id].lock);

            if (thread_array[id].open_list.size() > busy_threshold && thread_count > 1) {
                thread_array[id].wait_list.push(thread_array[id].open_list.top());
                thread_array[id].open_list.pop();
            }

            int current_open_size = thread_array[id].open_list.size();
            omp_unset_lock(&thread_array[id].lock);

            if (current_open_size == 0) {

//                for (auto neighbor: thread_array[id].neighbors) {
//                    omp_set_lock(&neighbor->lock);
//                }

                int busiest_neighbor = -1;
                int longest_wl = 0;
                for (int i = 0; i < thread_array[id].neighbors.size(); i++) {

                    auto neighbor = thread_array[id].neighbors[i];

                    omp_set_lock(&neighbor->lock);

                    if (neighbor->wait_list.size() > longest_wl) {
                        busiest_neighbor = i;
                        longest_wl = neighbor->wait_list.size();
                    }

                    omp_unset_lock(&neighbor->lock);
                }


                if (longest_wl > 0) {

                    omp_set_lock(&thread_array[id].neighbors[busiest_neighbor]->lock);

                    if (thread_array[id].neighbors[busiest_neighbor]->wait_list.size() > 0) {
                        auto fetch_element = thread_array[id].neighbors[busiest_neighbor]->wait_list.front();

                        thread_array[id].neighbors[busiest_neighbor]->wait_list.pop();

                        thread_array[id].open_list.push(fetch_element);
                        int copy_g_value = fetch_element.first - map->goal->compute_heuristic(fetch_element.second.second);
                        if (copy_g_value < g_value[fetch_element.second.second->node_id]) {
                            g_value[fetch_element.second.second->node_id] = copy_g_value;
                        }
                        omp_unset_lock(&thread_array[id].neighbors[busiest_neighbor]->lock);
                    }
                    else {
                        omp_unset_lock(&thread_array[id].neighbors[busiest_neighbor]->lock);
                        continue;
                    }
                }

                else {
                    continue;
                }
            }

            omp_set_lock(&thread_array[id].lock);
            Node* current_node = thread_array[id].open_list.top().second.second;
            Node* prev_node = thread_array[id].open_list.top().second.first;
            thread_array[id].open_list.pop();

            thread_array[id].count++;

            if (current_node->node_id == map->goal->node_id) {
                if (g_value[current_node->node_id] < optimal_length) {
                    optimal_length = g_value[current_node->node_id];
                }
            }

            // open_node_num--;

//            omp_unset_lock(&lock);

            omp_unset_lock(&thread_array[id].lock);

            delta_node_num -= 1;

            for (auto edge : current_node->adjacent_list)
            {
                Node *node = edge.first;
                if (prev_node!=NULL && prev_node->node_id==node->node_id) {
                    continue;
                }
                int weight = edge.second;

                int update_g_value = weight + g_value[current_node->node_id];

                if (update_g_value < g_value[node->node_id]) {
                    g_value[node->node_id] = update_g_value;
                    int new_f = update_g_value + map->goal->compute_heuristic(node);
                    thread_array[id].open_list.push({new_f, {current_node, node}});

                    delta_node_num++;
                }
            }
            // open_node_num += delta_node_num;
//            omp_set_lock(&lock);
        }

//        omp_unset_lock(&lock);
    }



    // for (int i = 0; i < thread_count; i++) {
    //     std::cout << "thread " << i << ": " << thread_array[i].count << std::endl;
    // }

    test_result->shortest = optimal_length;
    for (int i = 0; i < thread_count; i++) {
        test_result->thread_explore[i] = thread_array[i].count;
    }

    return test_result;
}