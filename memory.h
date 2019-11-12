#ifndef MEMORY_H
#define MEMORY_H

#include "simulator.h"

typedef struct partition {
    int base;
    int limit;
    int used;
} partition_t;
partition_t partitions[8];

void init_partitions();
int get_partition();

#endif
