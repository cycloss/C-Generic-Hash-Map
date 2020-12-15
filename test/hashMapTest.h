#include "../src/hashMap.h"

#define len(x) sizeof(x) / sizeof(x[0])

void printIntItem(void* item);
void printIntTable(hashMap* ht);
void printStrItem(void* item);
void printStrTable(hashMap* ht);