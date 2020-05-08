#include <iostream>
#include <queue>
#include <vector>
#include <unordered_set>

#include "map.h"
#include "sequential.h"

using namespace std;

TestResult *find_path_sequential(const Map *map, int thread_count)
{
    priority_queue<pair<int, Node *>> open_list;

    int goal_id = map->goal->node_id;
    open_list.push({map->goal->compute_heuristic(map->start), map->start});

    vector<int> g_value(map->height * map->width, INT32_MAX);
    // vector<int> path_parent(map->height*map->width, -1);

    g_value[map->start->node_id] = 0;

    int count = 0;

    clock_t total_cost = clock(), open_list_cost = 0;

    while (!open_list.empty())
    {
        count++;
        clock_t start, end;
        start = clock();
        Node *current_node = open_list.top().second;
        open_list.pop();
        open_list_cost += clock() - start;

        if (current_node->node_id == goal_id)
        {
            // cout << count << endl;

            // return g_value[current_node->node_id];
            break;
        }

        for (auto edge : current_node->adjacent_list)
        {
            Node *node = edge.first;
            int weight = edge.second;

            int update_g_value = weight + g_value[current_node->node_id];

            if (update_g_value < g_value[node->node_id])
            {
                g_value[node->node_id] = update_g_value;
                int new_f = update_g_value + map->goal->compute_heuristic(node);
                // path_parent[node->node_id] = current_node->node_id;
                start = clock();
                open_list.push({new_f, node});
                open_list_cost += clock() - start;
            }
        }
    }

    total_cost = clock() - total_cost;
    cout << "Sequential total time: " << total_cost << endl;
    cout << "Time on open_list maintanence: " << open_list_cost << endl;
    TestResult *ret = new TestResult(thread_count);
    ret->shortest = g_value[map->goal->node_id];
    ret->thread_explore[0] = count;
    return ret;
    // return INT32_MAX;
}
