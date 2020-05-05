//
// Created by Jikai Lu on 4/24/20.
//
#include "map.h"
#include "sequential.h"
#include "bidirectional.h"
#include "spa.h"
#include "hda.h"
#include "pla.h"

#include <string>
#include <chrono>
#include <fstream>

using namespace std;

typedef struct test_t
{
    chrono::_V2::steady_clock::time_point start, stop;
    TestResult* (*func)(const Map *map, int thread_count);
    string test_name;

    TestResult* result;

    int thread_count;

    test_t(TestResult* (*func)(const Map *map, int thread_count), string test_name, int thread_count)
    {
        this->func = func;
        this->test_name = test_name;
        this->thread_count = thread_count;
    }

    void execute(const Map *map)
    {
        // system("date");
        start = chrono::steady_clock::now();
        result = func(map, thread_count);
        stop = chrono::steady_clock::now();
        // system("date");
        // cout << "start " << start << " end " << stop << endl;
    }

    void show_result()
    {
        cout << test_name << " " << thread_count << endl;
        auto time_taken = chrono::duration_cast<chrono::milliseconds>(stop - start).count();
        cout << "Total Time : " << time_taken << "ms" << endl;
        cout << "shortest path length : " << result->shortest << endl;
        int sum = 0;
        for (int i = 0; i < thread_count; i ++) {
            // cout << "Thread " << i << " explores " << result->thread_explore[i] << " nodes";
            sum += result->thread_explore[i];
        }
        double mean = sum*1.0/thread_count;
        cout << "Explore " << sum << " nodes in total" << endl;
        cout << "Explore " << fixed << mean << " nodes in avg" << endl;
        cout << "Overal explore rate: " << fixed << sum*1.0/time_taken*1000 << endl;
        cout << "Avg explore rate: " << fixed << mean / time_taken*1000 << endl;

    }
} test_t;

int main(int argc, char** argv)
{
    if (argc < 2) {
        cout << "Please input maze file name" << endl;
        return 0;
    }

    cout << "Init" << endl;
    // string test_file_name = "maze_case/maze_1000_1000.txt";
    string test_file_name(argv[1]);
    ifstream input(test_file_name, ifstream::in);
    if (!input) {
        cout << "Bad input file: " << test_file_name << endl;
        return 0;
    } else {
        cout << "Loading input file: " << test_file_name << endl;
    }
    Map *map = new Map(input, false);
    cout << "Init done" << endl;

    vector<test_t> tests({
                        {&find_path_sequential, "Sequential", 1},
                          {&find_path_bidirectional, "Bidirection", 2},
                          {&find_path_bidirectional_custom, "Custom bidirection", 2},
                          {&find_path_spa, "SPA", 1},
                          {&find_path_spa, "SPA", 2},
                          {&find_path_spa, "SPA", 4},
                          {&find_path_spa, "SPA", 8},
                          {&find_path_spa, "SPA", 16},
                          {&find_path_spa, "SPA", 32},
                          {&find_path_spa_custom, "Custom SPA", 1},
                          {&find_path_spa_custom, "Custom SPA", 2},
                          {&find_path_spa_custom, "Custom SPA", 4},
                          {&find_path_spa_custom, "Custom SPA", 8},
                          {&find_path_spa_custom, "Custom SPA", 16},
                          {&find_path_spa_custom, "Custom SPA", 32},
                          {&find_path_hda_openmp, "HDA OpenMP", 1},
                          {&find_path_hda_openmp, "HDA OpenMP", 2},
                          {&find_path_hda_openmp, "HDA OpenMP", 4},
                          {&find_path_hda_openmp, "HDA OpenMP", 8},
                          {&find_path_hda_openmp, "HDA OpenMP", 16},
                          {&find_path_hda_openmp, "HDA OpenMP", 32},
                          {&find_path_hda_openmp_custom, "Custom HDA OpenMP", 1},
                          {&find_path_hda_openmp_custom, "Custom HDA OpenMP", 2},
                          {&find_path_hda_openmp_custom, "Custom HDA OpenMP", 4},
                          {&find_path_hda_openmp_custom, "Custom HDA OpenMP", 8},
                          {&find_path_hda_openmp_custom, "Custom HDA OpenMP", 16},
                          {&find_path_hda_openmp_custom, "Custom HDA OpenMP", 32},
                          {&find_path_pla, "PLA OpenMP", 1},
                          {&find_path_pla, "PLA OpenMP", 2},
                          {&find_path_pla, "PLA OpenMP", 4},
                          {&find_path_pla, "PLA OpenMP", 8},
                          {&find_path_pla, "PLA OpenMP", 16},
                          {&find_path_pla, "PLA OpenMP", 32}
                        });

    for (int i = 0; i < tests.size(); i++)
    {
        tests[i].execute(map);
        tests[i].show_result();
    }
}