#include <linkedList.h>
#include <stdbool.h>

#ifndef HASH_MAP_H
#define HASH_MAP_H

typedef struct {
    int _bucketCount;
    int _itemCount;
    unsigned int (*_keyHashFunction)(void*);
    bool (*_keyComparator)(void*, void*);
    linkedList** table;
} hashMap;

hashMap* createHashMap(unsigned int (*keyHashFunction)(void*), bool (*comparator)(void*, void*));
void addToMap(hashMap* hm, void* key, void* value, bool freeOnOverwrite);
keyValPair* removeKeyValPair(hashMap* hm, void* item);
bool mapContainsKey(hashMap* hm, void* key);
void* getValueForKey(hashMap* hm, void* key);
void clearMap(hashMap* hm, bool freeValues);
void freeMap(hashMap* hm, bool freeValues);
bool isEmptyMap(hashMap* hm);
void iterateMapPairs(hashMap* hm, void (*iterator)(keyValPair*));

#endif

#ifndef HASH_COMP_FUNCS
#define HASH_COMP_FUNCS
unsigned int intHash(void* val);
unsigned int strHash(void* str);
#endif