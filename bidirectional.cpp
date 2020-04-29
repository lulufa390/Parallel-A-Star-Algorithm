//
// Created by Jikai Lu on 4/24/20.
//

#include <pthread.h>

#include "map.h"
#include "bidirectional.h"

const int max_weight = 4096 * 4096;

struct myThreadFunArgs
{
    int id;
    const Map *map;
    std::vector<int> *g2_value;
    std::vector<int> *f2_value;
    int *f;

    // shared global variable
    long *current_shortest;
    //std::priority_queue<Node *> middle_list;
    std::unordered_set<int> *middle_list_id;

    pthread_mutex_t *lock;
};

static void *myThreadFun(void *vargp)
{
    struct myThreadFunArgs *args = (struct myThreadFunArgs *)vargp;

    const Map *map = args->map;

    std::vector<int> *g2_value = args->g2_value;
    std::vector<int> *f2_value = args->f2_value;

    std::unordered_set<int> &middle_list_id = *(args->middle_list_id);
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

    std::priority_queue<std::pair<int, Node *>> open_list;
    // g2_value[index][start->node_id] = 0;
    // f2_value[index][start->node_id] = goal->compute_heuristic(start);
    open_list.push({f2_value[index][start->node_id], start});

    int count = 0;

    while (!finished)
    {
        count++;
        Node *current_node = open_list.top().second;
        open_list.pop();

        if (middle_list_id.find(current_node->node_id) != middle_list_id.end())
        {
            if (f2_value[index][current_node->node_id] < current_shortest ||
                g2_value[index][current_node->node_id] + f[other] - start->compute_heuristic(current_node) < current_shortest)
            {

                for (auto edge : current_node->adjacent_list)
                {
                    Node *node = edge.first;
                    int weight = edge.second;

                    if (middle_list_id.find(node->node_id) != middle_list_id.end() || g2_value[index][node->node_id] > g2_value[index][current_node->node_id] + weight)
                    {

                        g2_value[index][node->node_id] = g2_value[index][current_node->node_id] + weight;
                        f2_value[index][node->node_id] = g2_value[index][node->node_id] + goal->compute_heuristic(node);

                        open_list.push({g2_value[index][node->node_id], node});

                        if (g2_value[0][node->node_id] + g2_value[1][node->node_id] < current_shortest)
                        {
                            pthread_mutex_lock(&lock);

                            if (g2_value[0][node->node_id] + g2_value[1][node->node_id] < current_shortest)
                            {
                                current_shortest = g2_value[0][node->node_id] + g2_value[1][node->node_id];
                                std::cout << current_shortest << std::endl;
                            }

                            pthread_mutex_unlock(&lock);
                        }
                    }
                }
            }

            middle_list_id.erase(current_node->node_id);
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

    std::cout << "thread " << index << " stops " << count << std::endl;

    return NULL;
}

int find_path_bidirectional(const Map *map)
{

    pthread_t tid1, tid2;

    std::vector<int> g2_value[2];
    std::vector<int> f2_value[2];

    int f[2];

    long current_shortest;
    std::unordered_set<int> middle_list_id;

    g2_value[0] = std::vector<int>(map->height * map->height, max_weight);
    g2_value[1] = std::vector<int>(map->height * map->height, max_weight);

    f2_value[0] = std::vector<int>(map->height * map->height, max_weight);
    f2_value[1] = std::vector<int>(map->height * map->height, max_weight);

    std::cout << g2_value[0].size() << std::endl;

    current_shortest = max_weight;

    for (Node *node : map->node_set)
    {
        // node->g2_value[0] = node->g2_value[1] = max_weight;
        middle_list_id.insert(node->node_id);
    }

    g2_value[0][map->start->node_id] = 0;
    g2_value[1][map->goal->node_id] = 0;

    f[0] = f2_value[0][map->start->node_id] = map->goal->compute_heuristic(map->start);
    f[1] = f2_value[1][map->goal->node_id] = map->start->compute_heuristic(map->goal);

    struct myThreadFunArgs args[2];

    pthread_mutex_t lock;

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
    }

    // args[1].id = 1;
    // args[1].map = map;
    // args[1].f2_value = f2_value;
    // args[1].g2_value = g2_value;
    // args[1].f = f;

    pthread_mutex_init(&lock, NULL);

    pthread_create(&tid1, NULL, myThreadFun, (void *)&args[0]);
    pthread_create(&tid2, NULL, myThreadFun, (void *)&args[1]);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    return current_shortest;
}
