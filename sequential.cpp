#include <iostream>
#include <queue>
#include <vector>
#include <unordered_set>

#include "map.h"
#include "sequential.h"

int find_path_sequential(const Map* map) {
    std::priority_queue<std::pair<int, Node *>> open_list;

    int goal_id = map->goal->node_id;

    open_list.push({map->goal->compute_heuristic(map->start) ,map->start});

    std::vector<int> g_value(map->height*map->width, INT32_MAX);
    std::vector<int> path_parent(map->height*map->width, -1);

    g_value[map->start->node_id] = 0;

    int count = 0;

    while (!open_list.empty()) {
        count ++;
        Node *current_node = open_list.top().second;
        open_list.pop();

        if (current_node->node_id == goal_id) {
            std::cout << count << std::endl;
            
            return g_value[current_node->node_id];
        }

        for (auto edge : current_node->adjacent_list)
        {
            Node *node = edge.first;
            int weight = edge.second;

            int update_g_value = weight + g_value[current_node->node_id];

            if (update_g_value < g_value[node->node_id]) {
                g_value[node->node_id] = update_g_value;
                int new_f = update_g_value + map->goal->compute_heuristic(node);
                path_parent[node->node_id] = current_node->node_id;
                open_list.push({new_f, node});
            }
        }
    }

    
}

