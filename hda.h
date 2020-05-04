#ifndef FINAL_DISTRIBUTED_H
#define FINAL_DISTRIBUTED_H
#include "map.h"

TestResult* find_path_hda_openmp(const Map* map, int);

TestResult* find_path_hda_openmp_custom(const Map *map, int thread_count);

#endif