#ifndef FINAL_DISTRIBUTED_H
#define FINAL_DISTRIBUTED_H
#include "map.h"

int find_path_hda_openmp(const Map* map, int);

int find_path_hda_openmp_custom(const Map *map, int thread_count);

#endif