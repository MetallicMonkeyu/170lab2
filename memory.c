#include "memory.h"


void init_partitions() {
    int i = 0;
    int partSize = 0;
    for (i = 0; i < 8; i++) {
       partitions[i].base = partSize;
       partSize += PageSize * 256;
       partitions[i].limit = PageSize * 256;
       //DEBUG('e', "8 %d\n", MemorySize/8);
       partitions[i].used = 0;
    }
}

int get_partition() {
   int i = 0;
   for (i = 0; i < 8; i++) {
       DEBUG('f', "partition %d %d\n", i, partitions[i].used);
       if (partitions[i].used == 0) {
           partitions[i].used = 1;
           DEBUG('e', "partition %d base %d limit %d\n", i, partitions[i].base, partitions[i].limit);
           return i;
       }
   }
   return -1;
}
