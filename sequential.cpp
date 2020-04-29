#include <iostream>
#include <queue>
#include <vector>
#include <unordered_set>

#include "map.h"
#include "sequential.h"

int find_path_sequential(Map* map) {
    std::priority_queue<std::pair<int, Node *>> open_list;

    std::unordered_set<int> open_id_list;
    std::unordered_set<int> close_id_list;

    int goal_id = map->goal->node_id;

    open_list.push({map->goal->compute_heuristic(map->start) ,map->start});
    open_id_list.insert(map->start->node_id);

    std::vector<int> f_value(map->height*map->width, INT32_MAX);
    std::vector<int> g_value(map->height*map->width, INT32_MAX);

    g_value[map->start->node_id] = 0;
    f_value[map->start->node_id] = map->goal->compute_heuristic(map->start);

    int count = 0;

    while (!open_list.empty()) {
        count ++;
        Node *current_node = open_list.top().second;
        open_list.pop();
        open_id_list.erase(current_node->node_id);
        close_id_list.insert(current_node->node_id);

        if (current_node->node_id == goal_id) {
            // print path
            Node *reverse_path = current_node;
            while (reverse_path->path_parent) {
                // std::cout << reverse_path->path_parent->x << " " << reverse_path->path_parent->y << std::endl;
                reverse_path = reverse_path->path_parent;
            }
            std::cout << count << std::endl;
            return g_value[current_node->node_id];
        }

        for (auto edge : current_node->adjacent_list)
        {
            Node *node = edge.first;
            int weight = edge.second;

            int update_g_value = weight + g_value[current_node->node_id];

            if (close_id_list.find(node->node_id) != close_id_list.end())
            {
                if (update_g_value < g_value[node->node_id])
                {
                    close_id_list.erase(node->node_id);
                    open_list.push({g_value[node->node_id], node});
                    open_id_list.insert(node->node_id);
                }
                else
                {
                    continue;
                }
            }
            else
            {
                if (open_id_list.find(node->node_id) == open_id_list.end())
                {
                    open_list.push({g_value[node->node_id], node});
                    open_id_list.insert(node->node_id);
                }
                else if (update_g_value > g_value[node->node_id])
                {
                    continue;
                }
            }

            g_value[node->node_id] = update_g_value;
            f_value[node->node_id] = update_g_value + map->goal->compute_heuristic(node);

            node->path_parent = current_node;
        }
    }

    
}


//int main()
//{
//    std::vector< std::vector<int> > mat1 = {
//        {0, 0, 2, 0, 0, 0, 0, 2, 0, 0},
//        {0, 0, 0, 0, 0, 0, 0, 2, 0, 0},
//        {0, 0, 2, 0, 0, 0, 0, 2, 0, 0},
//        {0, 0, 2, 0, 0, 0, 0, 2, 0, 0},
//        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//        {0, 2, 0, 0, 0, 0, 0, 2, 0, 0},
//        {1, 2, 0, 0, 0, 0, 0, 2, 0, 3},
//        {0, 2, 0, 0, 0, 0, 0, 2, 0, 0},
//        {0, 0, 0, 0, 0, 0, 0, 2, 0, 0},
//    };
//
//
////    std::vector< std::vector<int> > mat1 = {
////            {1, 0},
////            {0, 3},
////    };
//
//    std::vector< std::vector<int> > mat2 = {
//        {0, 0, 2, 0, 0, 0, 0, 2, 0, 0},
//        {0, 0, 0, 0, 0, 0, 0, 2, 0, 0},
//        {0, 0, 2, 0, 0, 0, 0, 2, 0, 0},
//        {0, 0, 2, 0, 0, 0, 0, 2, 0, 0},
//        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//        {0, 2, 0, 0, 0, 0, 0, 2, 0, 2},
//        {1, 2, 0, 0, 0, 0, 0, 2, 2, 2},
//        {0, 2, 0, 0, 0, 0, 0, 2, 0, 3},
//        {0, 0, 0, 0, 0, 0, 0, 2, 0, 0},
//    };
//
//    Map *map1 = new Map(mat1);
//
//    std::priority_queue<Node *> open_list;
//
//    std::unordered_set<int> open_id_list;
//    std::unordered_set<int> close_id_list;
//
//    open_list.push(map1->start);
//    open_id_list.insert(map1->start->node_id);
//
//    while (!open_list.empty()) {
//        Node *current_node = open_list.top();
//        open_list.pop();
//        open_id_list.erase(current_node->node_id);
//        close_id_list.insert(current_node->node_id);
//
//        if (current_node->is_goal) {
//            // print path
//            Node *reverse_path = current_node;
//            while (reverse_path->path_parent) {
//                std::cout << reverse_path->path_parent->x << " " << reverse_path->path_parent->y << std::endl;
//                reverse_path = reverse_path->path_parent;
//            }
//
//            return 0;
//        }
//
//        for (auto edge : current_node->adjacent_list)
//        {
//            Node *node = edge.first;
//            int weight = edge.second;
//
//            int update_g_value = weight + current_node->g_value;
//
//            if (close_id_list.find(node->node_id) != close_id_list.end())
//            {
//                if (update_g_value < node->g_value)
//                {
//                    close_id_list.erase(node->node_id);
//                    open_list.push(node);
//                    open_id_list.insert(node->node_id);
//                }
//                else
//                {
//                    continue;
//                }
//            }
//            else
//            {
//                if (open_id_list.find(node->node_id) == open_id_list.end())
//                {
//                    open_list.push(node);
//                    open_id_list.insert(node->node_id);
//                }
//                else if (update_g_value > node->g_value)
//                {
//                    continue;
//                }
//            }
//
//            node->g_value = update_g_value;
//            node->f_value = update_g_value + map1->compute_heuristic(node);
//
//            node->path_parent = current_node;
//        }
//    }
//
//    return 0;
//}