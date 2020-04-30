//
// Created by Jikai Lu on 4/24/20.
//
#include "map.h"
#include "sequential.h"
#include "bidirectional.h"
#include "centralized.h"

#include <string>

using namespace std;

typedef struct test_t
{
    clock_t start, stop;
    int (*func)(const Map *map, int thread_count);
    string test_name;

    int shortest;

    int thread_count;

    test_t(int (*func)(const Map *map, int thread_count), string test_name, int thread_count)
    {
        this->func = func;
        this->test_name = test_name;
        this->thread_count = thread_count;
    }

    void execute(const Map* map) {
        start = clock();
        shortest = func(map, thread_count);
        stop = clock();
    }

    void show_result()
    {
        cout << test_name << " Total Time : " << (double)(stop - start) / CLOCKS_PER_SEC << "s" << endl;
        cout << test_name <<" shortest path length : " << shortest << endl;
    }
} test_t;

int main()
{

    cout << "Init" << endl;
    string test_file_name = "maze_case/maze_2000_2000.txt";
    Map *map = new Map(test_file_name);
    cout << "Init done" << endl;

    vector<test_t> tests({
        {&find_path_sequential, "Sequential", 0},
        // {&find_path_bidirectional, "Bidirection", 2},
        // {&find_path_bidirectional_custom, "Custom bidirection", 2}
        {&find_path_spa, "SPA", 2}
        });

    for (int i = 0; i < tests.size(); i++)
    {
        tests[i].execute(map);
        tests[i].show_result();
    }

}