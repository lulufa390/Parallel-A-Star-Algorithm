//
// Created by Jikai Lu on 4/24/20.
//
#include "map.h"
#include "sequential.h"
#include "bidirectional.h"

#include <string>

typedef struct test_t
{
    clock_t start, stop;
    int (*func)(const Map *map);
    std::string test_name;

    int shortest;

    test_t(int (*func)(const Map *map), std::string test_name)
    {
        this->func = func;
        this->test_name = test_name;
    }

    void show_result()
    {
        std::cout << test_name << " Total Time : " << (double)(stop - start) / CLOCKS_PER_SEC << "s" << std::endl;
        std::cout << test_name <<" shortest path length : " << shortest << std::endl;
    }
} test_t;

int main()
{

    std::string test_file_name = "maze_case/maze_1000_1000.txt";
    Map *map = new Map(test_file_name);

    test_t tests[] = {
        {&find_path_sequential, "Sequential"},
        {&find_path_bidirectional, "Bidirection"},
        {&find_path_bidirectional_custom, "Custom bidirection"}};

    int test_count = 3;
    for (int i = 0; i < test_count; i++)
    {
        tests[i].start = clock();
        tests[i].shortest = tests[i].func(map);
        tests[i].stop = clock();
        tests[i].show_result();
    }

}