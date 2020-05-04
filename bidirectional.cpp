//
// Created by Jikai Lu on 4/24/20.
//

#include <pthread.h>
#include <cmath>

#include <tbb/concurrent_unordered_set.h>

#include "map.h"
#include "bidirectional.h"

using namespace std;

const int max_weight = 20000*20000;

struct bidirectionThreadArgs
{
    int id;
    const Map *map;
    vector<int> *g2_value;
    vector<int> *f2_value;
    int *f;

    // shared global variable
    long *current_shortest;
    //priority_queue<Node *> middle_list;
    tbb::concurrent_unordered_set<int> *middle_list_id;

    pthread_mutex_t *lock;

    int *count;
};

static void *bidirection_thread(void *vargp)
{
    struct bidirectionThreadArgs *args = (struct bidirectionThreadArgs *)vargp;

    const Map *map = args->map;

    vector<int> *g2_value = args->g2_value;
    vector<int> *f2_value = args->f2_value;

    tbb::concurrent_unordered_set<int> &middle_list_id = *(args->middle_list_id);
    long &current_shortest = *(args->current_shortest);

    pthread_mutex_t &lock = *(args->lock);

    int *f = args->f;

    bool finished = false;

    int index = args->id;
    int other = 1 - args->id;

    Node *start, *goal;
    if (args->id == 0)
    {
        start = map->start;
        goal = map->goal;
    }
    else
    {
        start = map->goal;
        goal = map->start;
    }

    priority_queue<pair<int, Node *>> open_list;
    // g2_value[index][start->node_id] = 0;
    // f2_value[index][start->node_id] = goal->compute_heuristic(start);
    open_list.push({f2_value[index][start->node_id], start});

    int count = 0;

    while (!finished)
    {
        
        Node *current_node = open_list.top().second;
        open_list.pop();

        if (middle_list_id.find(current_node->node_id) == middle_list_id.end())
        {
            if (f2_value[index][current_node->node_id] < current_shortest ||
                g2_value[index][current_node->node_id] + f[other] - start->compute_heuristic(current_node) < current_shortest)
            {

                for (auto edge : current_node->adjacent_list)
                {
                    Node *node = edge.first;
                    int weight = edge.second;

                    if (middle_list_id.find(node->node_id) == middle_list_id.end() || g2_value[index][node->node_id] > g2_value[index][current_node->node_id] + weight)
                    {

                        g2_value[index][node->node_id] = g2_value[index][current_node->node_id] + weight;
                        f2_value[index][node->node_id] = g2_value[index][node->node_id] + goal->compute_heuristic(node);

                        open_list.push({g2_value[index][node->node_id], node});
                        count++;

                        if (g2_value[0][node->node_id] + g2_value[1][node->node_id] < current_shortest)
                        {
                            pthread_mutex_lock(&lock);

                            if (g2_value[0][node->node_id] + g2_value[1][node->node_id] < current_shortest)
                            {
                                current_shortest = g2_value[0][node->node_id] + g2_value[1][node->node_id];
                                // cout << current_shortest << endl;
                            }

                            pthread_mutex_unlock(&lock);
                        }
                    }
                }
            }

            middle_list_id.insert(current_node->node_id);
            // middle_list_id.
        }

        if (open_list.size() > 0)
        {
            Node *peek_node = open_list.top().second;
            f[index] = f2_value[index][peek_node->node_id];
        }
        else
        {
            finished = true;
        }
    }

    args->count[index] = count;

    // cout << "thread " << index << " stops " << count << endl;

    return NULL;
}

TestResult* find_path_bidirectional(const Map *map, int thread_count)
{

    pthread_t tid1, tid2;

    vector<int> g2_value[2];
    vector<int> f2_value[2];

    int f[2];

    long current_shortest;
    tbb::concurrent_unordered_set<int> middle_list_id;

    g2_value[0] = vector<int>(map->height * map->height, max_weight);
    g2_value[1] = vector<int>(map->height * map->height, max_weight);

    f2_value[0] = vector<int>(map->height * map->height, max_weight);
    f2_value[1] = vector<int>(map->height * map->height, max_weight);

    // cout << g2_value[0].size() << endl;

    current_shortest = max_weight;

    // for (Node *node : map->node_set)
    // {
    //     // node->g2_value[0] = node->g2_value[1] = max_weight;
    //     middle_list_id.insert(node->node_id);
    // }

    g2_value[0][map->start->node_id] = 0;
    g2_value[1][map->goal->node_id] = 0;

    f[0] = f2_value[0][map->start->node_id] = map->goal->compute_heuristic(map->start);
    f[1] = f2_value[1][map->goal->node_id] = map->start->compute_heuristic(map->goal);

    struct bidirectionThreadArgs args[2];

    pthread_mutex_t lock;

    int count[2];

    for (int i = 0; i < 2; i++)
    {
        args[i].id = i;
        args[i].map = map;
        args[i].f2_value = f2_value;
        args[i].g2_value = g2_value;
        args[i].f = f;
        args[i].current_shortest = &current_shortest;
        args[i].middle_list_id = &middle_list_id;
        args[i].lock = &lock;
        args[i].count = count;
    }

    pthread_mutex_init(&lock, NULL);

    pthread_create(&tid1, NULL, bidirection_thread, (void *)&args[0]);
    pthread_create(&tid2, NULL, bidirection_thread, (void *)&args[1]);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    TestResult* ret = new TestResult(2);
    ret->shortest = current_shortest;
    ret->thread_explore[0] = count[0];
    ret->thread_explore[1] = count[1];

    return ret;
    // return current_shortest;
}

struct bidirectionCustomThreadArgs
{
    int id;
    const Map *map;
    vector<int> *g_value;

    int meet_id;

    int *count;
};

static void *bidirection_custom_thread(void *vargp)
{
    struct bidirectionCustomThreadArgs *args = (struct bidirectionCustomThreadArgs *)vargp;

    const Map *map = args->map;

    int id = args->id;

    int index = args->id;
    int other = 1 - args->id;

    Node *start, *goal;
    if (args->id == 0)
    {
        start = map->start;
        goal = map->goal;
    }
    else
    {
        start = map->goal;
        goal = map->start;
    }

    vector<int> *g_value = args->g_value;

    priority_queue<pair<int, Node *>> open_list;

    int goal_id = goal->node_id;

    open_list.push({goal->compute_heuristic(start), start});

    // vector<int> g_value(map->height * map->width, INT32_MAX);
    vector<int> path_parent(map->height * map->width, -1);

    g_value[index][start->node_id] = 0;

    int count = 0;

    while (!open_list.empty())
    {
        count++;
        Node *current_node = open_list.top().second;
        open_list.pop();

        if (g_value[other][current_node->node_id] < INT32_MAX)
        {
            args->meet_id = current_node->node_id;
            // cout << count << endl;
            // return NULL;
            break;
        }

        for (auto edge : current_node->adjacent_list)
        {
            Node *node = edge.first;
            int weight = edge.second;

            int update_g_value = weight + g_value[index][current_node->node_id];

            if (update_g_value < g_value[index][node->node_id])
            {
                g_value[index][node->node_id] = update_g_value;
                int new_f = update_g_value + map->goal->compute_heuristic(node);
                path_parent[node->node_id] = current_node->node_id;
                open_list.push({new_f, node});
            }
        }
    }
    args->count[id] = count;
}

TestResult* find_path_bidirectional_custom(const Map *map, int thread_count)
{

    pthread_t tid1, tid2;

    struct bidirectionCustomThreadArgs args[2];
    vector<int> g_value[2];

    int count[2] = {0};

    for (int i = 0; i < 2; i++)
    {
        g_value[i] = vector<int>(map->width * map->height, INT32_MAX);
        args[i].id = i;
        args[i].map = map;
        args[i].g_value = g_value;
        args[i].meet_id = -1;
        args[i].count = count;
    }

    pthread_create(&tid1, NULL, bidirection_custom_thread, (void *)&args[0]);
    pthread_create(&tid2, NULL, bidirection_custom_thread, (void *)&args[1]);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    TestResult* ret = new TestResult(2);
    ret->shortest = INT32_MAX;
    for (int i = 0; i < 2; i++)
    {
        if (args[i].meet_id >= 0)
        {
            int id = args[i].meet_id;
            int new_len = g_value[0][id] + g_value[1][id];
            if (new_len < ret->shortest)
            {
                ret->shortest = new_len;
            }
        }
        // cout << i << " thread takes " << count[i] << endl;
        ret->thread_explore[i] = count[i];
    }

    return ret;
}