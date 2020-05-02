#include "map.h"

#include <omp.h>
#include <tbb/concurrent_priority_queue.h>
#include <tbb/concurrent_queue.h>
#include <atomic>
#include <unordered_map>

using namespace std;

int hash_node(int node_id, int thread_count)
{
    return node_id % thread_count;
}

int find_path_hda_openmp(const Map *map, int thread_count)
{
    omp_set_num_threads(thread_count);
    bool finished = false;
    tbb::concurrent_priority_queue<pair<int, pair<int, Node *>>> queue[thread_count];
    int shortest = INT32_MAX;

    atomic_int remain_count;
    remain_count = 1;
    queue[hash_node(map->start->node_id, thread_count)].push({0, {0, map->start}});
#pragma omp parallel
    {
        int id = omp_get_thread_num();

        int count = 0;

        int goal_id = map->goal->node_id;

        tbb::concurrent_priority_queue<pair<int, pair<int, Node *>>> &open_list = queue[id];
        unordered_map<int, int> g_value;

        // g_value[map->start->node_id] = 0;

        if (id == 1)
        {
            // system("sleep 0.8");
        }

        while (!finished)
        {

            // system("sleep 0.8");

            // cout << id << " thread running " << endl;

            remain_count > 0;
            pair<int, pair<int, Node *>> item;
            bool has_item = open_list.try_pop(item);
            if (!has_item)
            {
                if (remain_count == 0)
                {
                    // return NULL;
                    cout << id << " thread break " << endl;
                    break;
                }
                continue;
            }

            count++;

            int current_g_value = item.second.first;
            Node *current_node = item.second.second;

            // cout << id << " thread handling " << current_node->node_id << endl;

            if (g_value.count(current_node->node_id) > 0 && current_g_value >= g_value[current_node->node_id])
            {
                // cout << id << " thread continue on " << current_node->node_id << endl;
                continue;
            }
            else
            {
                g_value[current_node->node_id] = current_g_value;
            }

            // cout << "thread " << thread_id << "running: " << count << endl;

            if (current_node->node_id == goal_id)
            {
                cout << count << endl;

                shortest = g_value[current_node->node_id];
                finished = true;
                cout << id << " thread found " << current_node->node_id << endl;
                // return NULL;
                break;
            }

            vector<pair<int, pair<int, Node *>>> expand_buffer;
            for (auto edge : current_node->adjacent_list)
            {
                Node *node = edge.first;
                int weight = edge.second;

                int update_g_value = weight + current_g_value;

                int new_f = update_g_value + map->goal->compute_heuristic(node);
                expand_buffer.push_back({new_f, {update_g_value, node}});
            }
            // pthread_mutex_lock(&lock);
            for (auto item : expand_buffer)
            {
                // open_list.push(item);
                queue[hash_node(item.second.second->node_id, thread_count)].push(item);
            }
            // pthread_mutex_unlock(&lock);
            remain_count += expand_buffer.size() - 1;
        }
    }
    return shortest;
}

// int main()
// {
// }