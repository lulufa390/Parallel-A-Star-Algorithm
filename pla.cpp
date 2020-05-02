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


//std::vector<std::vector<pair<int, Node *> > > distributed_nodes(std::vector<pair<int, Node *> > nodes, int b, int m) {
//    std::vector<std::vector<pair<int, Node *> > > group_vector(b, std::vector<pair<int, Node *> >());
//    srand(time(NULL));
//
//    for (int i = 0; i < b; i++) {
//        for (int j = 0 ; j < m; j++) {
//            group_vector[i].push_back(nodes[b*m+j]);
//        }
//    }
//
//    return group_vector;
//}

//void get_start_node(int b, int m, int thread_count) {
//
//    std::vector<pair<int, Node *>> start_list;
//
//    start_list.push_back({map->goal->compute_heuristic(map->start) ,map->start});
//
//    std::vector<pair<int, Node *>> expansion_list = node_split(start_list, b, m);
//
//
//
//    if (expansion_list.size() < b * m) {
//
//    }
//
//    while (expansion_list.size() < m * thread_count) {
//
//        std::vector<std::vector<pair<int, Node *> > > group_list = distributed_nodes(expansion_list, b, m);
//
//        expansion_list = std::vector<pair<int, Node *>>();
//
//        for (int i = 0; i < b; i++) {
//            std::vector<pair<int, Node *>> group_expansion_list = node_split(group_list[i], b, m);
//
//            for (auto element : group_expansion_list) {
//                expansion_list.push_back(element);
//            }
//        }
//    }
//}

void distribute_node(std::vector<pair<int, Node*> > node_list, int thread_count) {
    srand(time(NULL));
    for (int i = 0; i < node_list.size(); i++) {
        int thread_id = rand() % thread_count;
        thread_array[thread_id].open_list.push(node_list[i]);


//        thread_array[thread_id].g_value[node_list[i].second->node_id] =
    }
}


int compute_pla(const Map* map, int thread_count) {

    thread_array = new thread_state[thread_count];

//    for (int i = 0; i < thread_count; i++) {
//        thread_array->g_value = std::vector<int>(map->height*map->width, INT32_MAX);
//    }

    auto node_list = start_expand(map, 8);

    std::cout << "length: " << node_list.size() << std::endl;

    distribute_node(node_list, thread_count);

    omp_set_num_threads(thread_count);

//    best_soln = INT32_MAX;

    optimal_length = INT32_MAX;


    omp_lock_t lock;
    omp_init_lock(&lock);

#pragma omp parallel default(shared)
    {
//        priority_queue<pair<int, Node *>> open_list;

//        int goal_id = map->goal->node_id;
//        open_list.push({map->goal->compute_heuristic(map->start) ,map->start});

        vector<int> g_value(start_g_value);

//        for (int i =)
        // vector<int> path_parent(map->height*map->width, -1);

//        g_value[map->start->node_id] = 0;
        int id = omp_get_thread_num();

//        std::cout << id << std::endl;

//        int count = 0;


        omp_set_lock(&lock);
        while (!thread_array[id].open_list.empty() || wait_list.size() != 0) {
//            count++;

//            omp_unset_lock(&lock);

//            Node* current_node = thread_array[id].open_list.top().second;
//            thread_array[id].open_list.pop();

            if (thread_array[id].open_list.size() > 5) {
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


            if (current_node->node_id == map->goal->node_id) {
//                optimal_length = g_value[current_node->node_id];

                if (g_value[current_node->node_id] < optimal_length) {
                    optimal_length = g_value[current_node->node_id];
                }
//                cout << count << endl;

//                return best_soln;
            }
            omp_unset_lock(&lock);

//            if (g_value[current_node->node_id] + map->goal->compute_heuristic(current_node) > best_soln) {
//
//                std::cout << "best: " << best_soln << std::endl;
//
//                std::cout << "g_value: " << g_value[current_node->node_id] << std::endl;
//                std::cout << "h_value: " << map->goal->compute_heuristic(current_node) << std::endl;
//
//
//                continue;
//            }

            for (auto edge : current_node->adjacent_list)
            {
                Node *node = edge.first;
                int weight = edge.second;

                int update_g_value = weight + g_value[current_node->node_id];

                if (update_g_value < g_value[node->node_id]) {
                    g_value[node->node_id] = update_g_value;
                    int new_f = update_g_value + map->goal->compute_heuristic(node);

//                    if (new_f < best_soln) {
//                        best_soln = new_f;
//                    }

                    // path_parent[node->node_id] = current_node->node_id;
                    thread_array[id].open_list.push({new_f, node});
                }
            }
            omp_set_lock(&lock);
        }

        omp_unset_lock(&lock);
    }

    return optimal_length;
}