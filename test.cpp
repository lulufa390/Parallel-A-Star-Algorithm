//
// Created by Jikai Lu on 4/24/20.
//
#include "map.h"
#include "sequential.h"
#include "bidirectional.h"
#include "centralized.h"
#include "distributed.h"

#include <string>
#include <chrono>

using namespace std;

typedef struct test_t
{
    chrono::_V2::steady_clock::time_point start, stop;
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

    void execute(const Map *map)
    {
        // system("date");
        start = chrono::steady_clock::now();
        shortest = func(map, thread_count);
        stop = chrono::steady_clock::now();
        // system("date");
        // cout << "start " << start << " end " << stop << endl;
    }

    void show_result()
    {
        cout << test_name << " " << thread_count << endl;
        cout << " Total Time : " << chrono::duration_cast<chrono::milliseconds>(stop - start).count() << "ms" << endl;
        cout << " shortest path length : " << shortest << endl;
    }
} test_t;

int main()
{

    cout << "Init" << endl;
    string test_file_name = "maze_case/maze_1000_1000.txt";
    Map *map = new Map(test_file_name);
    cout << "Init done" << endl;

    vector<test_t> tests({{&find_path_sequential, "Sequential", 0},
                          {&find_path_bidirectional, "Bidirection", 2},
                          {&find_path_bidirectional_custom, "Custom bidirection", 2},
                          {&find_path_hda_openmp, "SPA", 1},
                          {&find_path_spa, "SPA", 2},
                          {&find_path_spa, "SPA", 4},
                          {&find_path_hda_openmp, "HDA OpenMP", 1},
                          {&find_path_hda_openmp, "HDA OpenMP", 2},
                          {&find_path_hda_openmp, "HDA OpenMP", 4}});

    for (int i = 0; i < tests.size(); i++)
    {
        tests[i].execute(map);
        tests[i].show_result();
    }
}