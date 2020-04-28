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


void *myThreadFun(void *vargp) {
    Map *map = (Map *) vargp;


    std::unordered_set<int> open_list_id;
    open_list_id.insert(map->start->node_id);


    bool finished = false;

    int index = map->is_reverse ? 1:0;
    int other = map->is_reverse ? 0:1;

    auto compare = [index] (Node* a, Node* b) -> bool {
        return a->f2_value[index] > b->f2_value[index];
    };
    std::priority_queue<Node *, std::vector<Node*>, decltype(compare)> open_list(compare);
    open_list.push(map->start);

    while (!finished) {
        std::cout << "thread " << index << std::endl;
        Node *current_node = open_list.top();
        open_list.pop();
        open_list_id.erase(current_node->node_id);

        if (middle_list_id.find(current_node->node_id) != middle_list_id.end())
        {
            if (current_node->f2_value[index] < current_shortest ||
                current_node->g2_value[index] + f[other] - map->compute_heuristic_reverse(current_node) < current_shortest)
            {

                for (auto edge : current_node->adjacent_list) {
                    Node *node = edge.first;
                    int weight = edge.second;

                    if (middle_list_id.find(node->node_id) != middle_list_id.end() || node->g2_value[index] > current_node->g2_value[index] + weight)
                    {

                        node->g2_value[index] = current_node->g2_value[index] + weight;
                        node->f2_value[index] = node->g2_value[index] + map->compute_heuristic(node);

                        open_list_id.insert(node->node_id);
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

        if (open_list_id.size() > 0) {
            Node* peek_node = open_list.top();
            f[index] = peek_node->f2_value[index];
        }
        else {
            finished = true;
        }

    }

    return NULL;
}


int find_path_bidirectional(Map* map) {

    pthread_t tid1, tid2;
    Map *map2 = new Map(*map);
    map2->reverse();

    current_shortest = max_weight;

    for (Node *node : map->node_set) {
        node->g2_value[0] = node->g2_value[1] = max_weight;

        if (node == map->start) {
            node->g2_value[0] = 0;
            f[0] = node->f2_value[0] = map->compute_heuristic(node);
        }

        if (node == map2->start) {
            node->g2_value[1] = 0;

            f[1] = node->f2_value[1] = map2->compute_heuristic(node);
        }

        middle_list_id.insert(node->node_id);
    }

    pthread_mutex_init(&lock, NULL);


    pthread_create(&tid1, NULL, myThreadFun, (void *) map);
    pthread_create(&tid2, NULL, myThreadFun, (void *) map2);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    return current_shortest;

}

//int main() {
//
//    pthread_t tid1, tid2;
//
//    std::vector<std::vector<int> > mat = {
//            {0, 1, 2, 0, 0, 0, 0, 2, 0, 0},
//            {0, 0, 0, 0, 0, 0, 0, 2, 0, 0},
//            {0, 0, 2, 0, 0, 0, 0, 2, 0, 0},
//            {0, 0, 2, 0, 0, 0, 0, 2, 0, 0},
//            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//            {0, 2, 0, 0, 0, 0, 0, 2, 0, 0},
//            {0, 2, 0, 0, 0, 0, 0, 2, 0, 3},
//            {0, 2, 0, 0, 0, 0, 0, 2, 0, 0},
//            {0, 0, 0, 0, 0, 0, 0, 2, 0, 0},
//    };
//
//    Map *map1 = new Map(mat);
//    Map *map2 = new Map(*map1);
//    map2->reverse();
//
//    // initialize global variables
//    current_shortest = max_weight;
//
//    for (Node *node : map1->node_set) {
//        node->g2_value[0] = node->g2_value[1] = max_weight;
//
//        if (node == map1->start) {
//            node->g2_value[0] = 0;
//            f[0] = node->f2_value[0] = map1->compute_heuristic(node);
//        }
//
//        if (node == map2->start) {
//            node->g2_value[1] = 0;
//
//            f[1] = node->f2_value[1] = map2->compute_heuristic(node);
//        }
//
//        middle_list_id.insert(node->node_id);
//    }
//
//    pthread_mutex_init(&lock, NULL);
//
//
//    pthread_create(&tid1, NULL, myThreadFun, (void *) map1);
//    pthread_create(&tid2, NULL, myThreadFun, (void *) map2);
//
//    pthread_join(tid1, NULL);
//    pthread_join(tid2, NULL);
//
//    std::cout << current_shortest << std::endl;
//
//    return 0;
//}