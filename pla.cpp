//
// Created by Jikai Lu on 4/30/20.
//

#include "pla.h"

int optimal_length;
thread_state* thread_array;
std::vector<int> start_g_value;
std::queue<pair<int, Node *>> wait_list;

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
        thread_array[thread_id].open_list.push(node_list[i]);
    }
}


int find_path_pla(const Map* map, int thread_count) {

    auto node_list = start_expand(map, thread_count);

    const int busy_threshold = 5;

    if (node_list.empty()) {
        std::cout << "do not need multi threads, the map is too small" << std::endl;
        return optimal_length;
//        thread_array = new thread_state[1];
//        thread_array[1].open_list.push({0, map->start});
//        omp_set_num_threads(1);
    }
    else {
        thread_array = new thread_state[thread_count];
        distribute_node(node_list, thread_count);
        omp_set_num_threads(thread_count);
    }

    optimal_length = INT32_MAX;

    omp_lock_t lock;
    omp_init_lock(&lock);


#pragma omp parallel default(shared)
    {

        int count = 0;
        vector<int> g_value(start_g_value);

        int id = omp_get_thread_num();

        omp_set_lock(&lock);
        while (!thread_array[id].open_list.empty() || wait_list.size() != 0) {

            if (thread_array[id].open_list.size() > busy_threshold) {
                wait_list.push(thread_array[id].open_list.top());
                thread_array[id].open_list.pop();
            }

            Node* current_node = nullptr;

            if (thread_array[id].open_list.size() == 0) {
                if (wait_list.size() > 0) {
                    auto fetch_element = wait_list.front();
                    wait_list.pop();

                    thread_array[id].open_list.push(fetch_element);

                    int copy_g_value = fetch_element.first - map->goal->compute_heuristic(fetch_element.second);

                    if (copy_g_value < g_value[fetch_element.second->node_id]) {
                        g_value[fetch_element.second->node_id] = copy_g_value;
                    }

                }

                current_node = thread_array[id].open_list.top().second;
                thread_array[id].open_list.pop();
            }
            else {
                current_node = thread_array[id].open_list.top().second;
                thread_array[id].open_list.pop();
            }

            count ++;


            if (current_node->node_id == map->goal->node_id) {
                if (g_value[current_node->node_id] < optimal_length) {
                    optimal_length = g_value[current_node->node_id];
                }
            }
            omp_unset_lock(&lock);


            for (auto edge : current_node->adjacent_list)
            {
                Node *node = edge.first;
                int weight = edge.second;

                int update_g_value = weight + g_value[current_node->node_id];

                if (update_g_value < g_value[node->node_id]) {
                    g_value[node->node_id] = update_g_value;
                    int new_f = update_g_value + map->goal->compute_heuristic(node);
                    thread_array[id].open_list.push({new_f, node});
                }
            }
            omp_set_lock(&lock);
        }

        omp_unset_lock(&lock);

        cout << "thread " << id << " count " << count << endl;
    }

    return optimal_length;
}