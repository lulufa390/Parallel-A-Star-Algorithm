//
// Created by Jikai Lu on 4/24/20.
//

#include <pthread.h>


#include "map.h"
#include "bidirectional.h"


// shared global variable
long current_shortest;
//std::priority_queue<Node *> middle_list;
std::unordered_set<int> middle_list_id;
int f[2];

pthread_mutex_t lock;

const int max_weight = 4096 * 4096;

struct myThreadFunArgs {
    int id;
    Map *map;
};


static void *myThreadFun(void *vargp) {
    struct myThreadFunArgs *args = (struct myThreadFunArgs *) vargp;

    Map* map = args->map;




    bool finished = false;

    int index = args->id;
    int other = 1 - args->id;

    Node *start, *goal;
    if (args->id == 0) {
        start = map->start;
        goal = map->goal;
    } else {
        start = map->goal;
        goal = map->start;
    }

    auto compare = [index] (Node* a, Node* b) -> bool {
        return a->f2_value[index] > b->f2_value[index];
    };
    std::priority_queue<Node *, std::vector<Node*>, decltype(compare)> open_list(compare);
    open_list.push(start);

    int count = 0;

    while (!finished) {
        count ++;
        Node *current_node = open_list.top();
        open_list.pop();

        if (middle_list_id.find(current_node->node_id) != middle_list_id.end())
        {
            if (current_node->f2_value[index] < current_shortest ||
                current_node->g2_value[index] + f[other] - start->compute_heuristic(current_node) < current_shortest)
            {

                for (auto edge : current_node->adjacent_list) {
                    Node *node = edge.first;
                    int weight = edge.second;

                    if (middle_list_id.find(node->node_id) != middle_list_id.end() || node->g2_value[index] > current_node->g2_value[index] + weight)
                    {

                        node->g2_value[index] = current_node->g2_value[index] + weight;
                        node->f2_value[index] = node->g2_value[index] + goal->compute_heuristic(node);

                        open_list.push(node);


                        if (node->g2_value[0] + node->g2_value[1] < current_shortest) {
                            pthread_mutex_lock(&lock);

                            if (node->g2_value[0] + node->g2_value[1] < current_shortest) {
                                current_shortest = node->g2_value[0] + node->g2_value[1];
                            }

                            pthread_mutex_unlock(&lock);
                        }

                    }

                }
            }

            middle_list_id.erase(current_node->node_id);
        }

        if (open_list.size() > 0) {
            Node* peek_node = open_list.top();
            f[index] = peek_node->f2_value[index];
        }
        else {
            finished = true;
        }

    }

    std::cout << "thread " << index << " stops " << count  << std::endl;

    return NULL;
}


int find_path_bidirectional(Map* map) {

    pthread_t tid1, tid2;

    

    current_shortest = max_weight;

    for (Node *node : map->node_set) {
        node->g2_value[0] = node->g2_value[1] = max_weight;
        middle_list_id.insert(node->node_id);
    }

    map->start->g2_value[0] = 0;
    map->goal->g2_value[1] = 0;

    f[0] = map->start->f2_value[0] = map->goal->compute_heuristic(map->start);
    f[1] = map->goal->f2_value[1] = map->start->compute_heuristic(map->goal);

    struct myThreadFunArgs args[2];
    args[0].id = 0;
    args[0].map = map;

    args[1].id = 1;
    args[1].map = map;

    pthread_mutex_init(&lock, NULL);


    pthread_create(&tid1, NULL, myThreadFun, (void *) &args[0]);
    pthread_create(&tid2, NULL, myThreadFun, (void *) &args[1]);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    return current_shortest;

}
