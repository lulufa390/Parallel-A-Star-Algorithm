#include "spa.h"

#include <pthread.h>
#include <atomic>
#include <iostream>

#include <tbb/concurrent_priority_queue.h>

using namespace std;

struct spaCustomThreadArgs
{
    int id;
    const Map *map;

    bool *finished;
    long *shortest;

    vector<int> *g_value;

    pthread_mutex_t *lock;
    priority_queue<pair<int, Node *>> *open_list;
    int *remain_count;

    int *count;
};

static void *spa_custom_thread(void *vargp)
{
    struct spaCustomThreadArgs *args = (struct spaCustomThreadArgs *)vargp;

    int thread_id = args->id;
    const Map *map = args->map;

    long &shortest = *(args->shortest);
    bool &finished = *(args->finished);

    vector<int> &g_value = *(args->g_value);

    pthread_mutex_t &lock = *(args->lock);
    priority_queue<pair<int, Node *>> &open_list = *(args->open_list);
    int &remain_count = *(args->remain_count);

    int count = 0;

    int goal_id = map->goal->node_id;

    queue<pair<int, Node *>> local_open_list;
    vector<pair<int, Node *>> expand_buffer;
    int local_take_count = 0;

    const int MAX_PREFETCH_COUNT = 3;
    const int MAX_EXPAND_COUNT = 3;

    while (!finished && remain_count > 0)
    {
        // if (count % 1000 == 0)
        // cout << thread_id << " thread " << remain_count << " " << count << endl;

        if (open_list.size() == 0 || expand_buffer.size() > MAX_EXPAND_COUNT)
        {
            pthread_mutex_lock(&lock);
            remain_count += expand_buffer.size() - local_take_count;
            for (auto item : expand_buffer)
            {
                open_list.push(item);
            }
            pthread_mutex_unlock(&lock);
            // cout << thread_id << " thread flush " << expand_buffer.size() << " " << local_take_count << endl;
            local_take_count = 0;
            expand_buffer.clear();
        }

        if (local_open_list.size() == 0)
        {
            pthread_mutex_lock(&lock);
            if (open_list.empty())
            {
                pthread_mutex_unlock(&lock);
                continue;
            }
            int prefetch_count = 0;
            while (prefetch_count < MAX_PREFETCH_COUNT && open_list.size() > 0)
            {
                auto item = open_list.top();
                open_list.pop();
                local_open_list.push(item);
                prefetch_count++;
            }
            pthread_mutex_unlock(&lock);
            // cout << thread_id << " thread take " << endl;
        }

        Node *current_node = local_open_list.front().second;
        local_open_list.pop();

        local_take_count++;

        count++;

        // cout << "thread " << thread_id << "running: " << count << endl;

        if (current_node->node_id == goal_id)
        {

            shortest = g_value[current_node->node_id];
            finished = true;
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
                expand_buffer.push_back({new_f, node});
            }
        }
        // cout << thread_id << " expand " << expand_buffer.size() << endl;
    }

    // cout << "thread " << thread_id << " done" << endl;

    // cout << count << endl;
    args->count[thread_id] = count;

    return NULL;
}

TestResult *find_path_spa_custom(const Map *map, int thread_count)
{
    pthread_t tid[thread_count];

    // shared variables
    long shortest = INT32_MAX;
    bool finished = false;
    ;
    vector<int> g_value = vector<int>(map->height * map->height, INT32_MAX);
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);
    priority_queue<pair<int, Node *>> open_list;
    int remain_count;

    struct spaCustomThreadArgs args[thread_count];
    int count[thread_count];

    for (int i = 0; i < thread_count; i++)
    {
        args[i].id = i;
        args[i].map = map;
        args[i].finished = &finished;
        args[i].shortest = &shortest;
        args[i].g_value = &g_value;
        args[i].lock = &lock;
        args[i].open_list = &open_list;
        args[i].remain_count = &remain_count;
        args[i].count = count;
    }

    int goal_id = map->goal->node_id;
    open_list.push({map->goal->compute_heuristic(map->start), map->start});

    g_value[map->start->node_id] = 0;
    remain_count = 1;

    for (int i = 0; i < thread_count; i++)
    {
        pthread_create(&tid[i], NULL, spa_custom_thread, (void *)&args[i]);
    }

    for (int i = 0; i < thread_count; i++)
    {
        pthread_join(tid[i], NULL);
    }

    TestResult *ret = new TestResult(thread_count);
    ret->shortest = shortest;
    for (int i = 0; i < thread_count; i++)
    {
        ret->thread_explore[i] = count[i];
    }

    return ret;
}

struct spaThreadArgs
{
    int id;
    const Map *map;

    bool *finished;
    long *shortest;

    vector<int> *g_value;

    pthread_mutex_t *lock;
    tbb::concurrent_priority_queue<pair<int, Node *>> *open_list;
    atomic_int *remain_count;

    int *count;
};

static void *spa_thread(void *vargp)
{
    struct spaThreadArgs *args = (struct spaThreadArgs *)vargp;

    int thread_id = args->id;
    const Map *map = args->map;

    long &shortest = *(args->shortest);
    bool &finished = *(args->finished);

    vector<int> &g_value = *(args->g_value);

    // pthread_mutex_t &lock = *(args->lock);
    tbb::concurrent_priority_queue<pair<int, Node *>> &open_list = *(args->open_list);
    atomic_int &remain_count = *(args->remain_count);

    int count = 0;

    int goal_id = map->goal->node_id;

    while (!finished)
    {

        // pthread_mutex_lock(&lock);
        // if (open_list.empty()) {
        //     pthread_mutex_unlock(&lock);
        //     continue;
        // }
        // Node *current_node = open_list.top().second;
        // open_list.pop();
        // pthread_mutex_unlock(&lock);

        remain_count > 0;
        pair<int, Node *> item;
        bool has_item = open_list.try_pop(item);
        if (!has_item)
        {
            if (remain_count == 0)
            {
                // return NULL;
                break;
            }
            continue;
        }

        Node *current_node = item.second;
        count++;

        // cout << "thread " << thread_id << "running: " << count << endl;

        if (current_node->node_id == goal_id)
        {
            // cout << count << endl;

            shortest = g_value[current_node->node_id];
            finished = true;
            // return NULL;
            break;
        }

        vector<pair<int, Node *>> expand_buffer;
        for (auto edge : current_node->adjacent_list)
        {
            Node *node = edge.first;
            int weight = edge.second;

            int update_g_value = weight + g_value[current_node->node_id];

            if (update_g_value < g_value[node->node_id])
            {
                g_value[node->node_id] = update_g_value;
                int new_f = update_g_value + map->goal->compute_heuristic(node);
                expand_buffer.push_back({new_f, node});
            }
        }
        // pthread_mutex_lock(&lock);
        for (auto item : expand_buffer)
        {
            open_list.push(item);
        }
        // pthread_mutex_unlock(&lock);
        remain_count += expand_buffer.size() - 1;
    }

    // cout << count << endl;
    args->count[thread_id] = count;

    return NULL;
}

TestResult *find_path_spa(const Map *map, int thread_count)
{
    pthread_t tid[thread_count];

    // shared variables
    long shortest = INT32_MAX;
    bool finished = false;
    ;
    vector<int> g_value = vector<int>(map->height * map->height, INT32_MAX);
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);
    tbb::concurrent_priority_queue<pair<int, Node *>> open_list;
    atomic_int remain_count;

    struct spaThreadArgs args[thread_count];
    int count[thread_count];

    for (int i = 0; i < thread_count; i++)
    {
        args[i].id = i;
        args[i].map = map;
        args[i].finished = &finished;
        args[i].shortest = &shortest;
        args[i].g_value = &g_value;
        args[i].lock = &lock;
        args[i].open_list = &open_list;
        args[i].remain_count = &remain_count;
        args[i].count = count;
    }

    int goal_id = map->goal->node_id;
    open_list.push({map->goal->compute_heuristic(map->start), map->start});

    g_value[map->start->node_id] = 0;
    remain_count = 1;

    for (int i = 0; i < thread_count; i++)
    {
        pthread_create(&tid[i], NULL, spa_thread, (void *)&args[i]);
    }

    for (int i = 0; i < thread_count; i++)
    {
        pthread_join(tid[i], NULL);
    }

    TestResult *ret = new TestResult(thread_count);
    ret->shortest = shortest;
    for (int i = 0; i < thread_count; i++)
    {
        ret->thread_explore[i] = count[i];
    }

    // return shortest;
    return ret;
}