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


TestResult* find_path_hda_openmp(const Map *map, int thread_count)
{
    omp_set_num_threads(thread_count);
    bool finished = false;
    tbb::concurrent_queue<pair<int, pair<Node *, Node *>>> queue[thread_count];
    // int shortest = INT32_MAX;

    atomic_int remain_count;
    remain_count = 1;
    queue[hash_node(map->start->node_id, thread_count)].push({map->goal->compute_heuristic(map->start), {NULL, map->start}});
    // int count_array[thread_count];
    TestResult* ret = new TestResult(thread_count);
    ret->shortest = INT32_MAX;
#pragma omp parallel
    {
        int id = omp_get_thread_num();

        int count = 0;

        int goal_id = map->goal->node_id;

        priority_queue<pair<int, pair<Node *, Node *>>> open_list;
        unordered_map<int, int> g_value;

        while (!finished)
        {

            while (1)
            {
                pair<int, pair<Node *, Node *>> item;
                bool has_item = queue[id].try_pop(item);
                if (has_item) {
                    open_list.push(item);
                } else {
                    break;
                }
            }

            if (open_list.size() == 0)
            {
                if (remain_count == 0)
                {
                    break;
                }
                continue;
            }

            pair<int, pair<Node *, Node *>> item = open_list.top();
            open_list.pop();


            count++;

            Node *prev_node = item.second.first;
            Node *current_node = item.second.second;
            int current_g_value = item.first - map->goal->compute_heuristic(current_node);

            // cout << id << " thread handling " << current_node->node_id << endl;

            if (g_value.count(current_node->node_id) > 0 && current_g_value >= g_value[current_node->node_id])
            {
                remain_count.fetch_add(-1);
                continue;
            }
            else
            {
                g_value[current_node->node_id] = current_g_value;
            }

            if (current_node->node_id == goal_id)
            {
                ret->shortest = g_value[current_node->node_id];
                finished = true;
                break;
            }

            vector<pair<int, pair<Node *, Node *>>> expand_buffer;
            for (auto edge : current_node->adjacent_list)
            {
                Node *node = edge.first;
                if (prev_node != NULL && prev_node->node_id == node->node_id)
                {
                    continue;
                }
                int weight = edge.second;

                int update_g_value = weight + current_g_value;

                int new_f = update_g_value + map->goal->compute_heuristic(node);
                expand_buffer.push_back({new_f, {current_node, node}});
            }
            remain_count.fetch_add(expand_buffer.size() - 1);
            for (auto item : expand_buffer)
            {
                queue[hash_node(item.second.second->node_id, thread_count)].push(item);
            }
        }
        ret->thread_explore[id] = count;
    }
    // for (int i = 0; i < thread_count; i++)
    // {
    //     cout << i << " thread takes " << count_array[i] << endl;
    // }
    return ret;
}


TestResult* find_path_hda_openmp_custom(const Map *map, int thread_count)
{
    omp_set_num_threads(thread_count);
    bool finished = false;
    tbb::concurrent_priority_queue<pair<int, pair<Node*, Node *>>> queue[thread_count];
    // int shortest = INT32_MAX;

    atomic_int remain_count;
    remain_count = 1;
    queue[hash_node(map->start->node_id, thread_count)].push({map->goal->compute_heuristic(map->start), {NULL, map->start}});
    // int count_array[thread_count];
    TestResult* ret = new TestResult(thread_count);
    ret->shortest = INT32_MAX;
#pragma omp parallel
    {
        int id = omp_get_thread_num();

        int count = 0;

        int goal_id = map->goal->node_id;

        tbb::concurrent_priority_queue<pair<int, pair<Node*, Node *>>> &open_list = queue[id];
        unordered_map<int, int> g_value;


        while (!finished)
        {

// if (id == 0)
            // cout << id << " thread running " << remain_count << endl;
            pair<int, pair<Node*, Node *>> item;
            bool has_item = open_list.try_pop(item);
            if (!has_item)
            {
                if (remain_count == 0)
                {
                    // return NULL;
                    // cout << id << " thread break " << endl;
                    break;
                }
                continue;
            }

            count++;


            Node *prev_node = item.second.first;
            Node *current_node = item.second.second;
            int current_g_value = item.first - map->goal->compute_heuristic(current_node);

            // cout << id << " thread handling " << current_node->node_id << endl;

            if (g_value.count(current_node->node_id) > 0 && current_g_value >= g_value[current_node->node_id])
            {
                remain_count.fetch_add(-1);
                continue;
            }
            else
            {
                g_value[current_node->node_id] = current_g_value;
            }

            if (current_node->node_id == goal_id)
            {
                ret->shortest = g_value[current_node->node_id];
                finished = true;
                break;
            }

            vector<pair<int, pair<Node*, Node *>>> expand_buffer;
            for (auto edge : current_node->adjacent_list)
            {
                Node *node = edge.first;
                if (prev_node!=NULL&&prev_node->node_id==node->node_id) {
                    continue;
                }
                int weight = edge.second;

                int update_g_value = weight + current_g_value;

                int new_f = update_g_value + map->goal->compute_heuristic(node);
                expand_buffer.push_back({new_f, {current_node, node}});
            }
            remain_count.fetch_add(expand_buffer.size() - 1);
            for (auto item : expand_buffer)
            {
                queue[hash_node(item.second.second->node_id, thread_count)].push(item);
            }
        }
        ret->thread_explore[id] = count;
    }
    // for (int i = 0; i < thread_count; i ++) {
    //     cout << i << " thread takes " << count_array[i] << endl;
    // }
    return ret;
}

// int main()
// {
// }