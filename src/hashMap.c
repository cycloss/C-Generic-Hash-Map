#include "hashMap.h"
#include <linkedList.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
//TODO write readme
//TODO reorder functions

/**
 * A copy of handleRemoval in linkedList. Required to keep it static.
 */
static void handleRemoval(linkedList* l, node* previous, node* current) {
    if (current == l->head) {
        l->head = current->next;
    }
    if (current == l->tail) {
        l->tail = previous;
    }
    if (previous) {
        previous->next = current->next;
    }
    l->_size--;
}

static keyValPair* removeForKey(linkedList* l, void* keyp, bool (*keyComparator)(void*, void*)) {
    node* current = l->head;
    node* previous = NULL;
    while (current) {
        keyValPair* kvpp = current->value;
        if (keyComparator(keyp, kvpp->key)) {
            handleRemoval(l, previous, current);
            free(current);
            return kvpp;
        }
        previous = current;
        current = current->next;
    }
    return NULL;
}

static int findIndexForKey(linkedList* l, void* keyp, bool (*keyComparator)(void*, void*)) {
    node* current = l->head;
    for (int i = 0; current; current = current->next, i++) {
        keyValPair* kvpp = current->value;
        if (keyComparator(keyp, kvpp->key)) {
            return i;
        }
    }
    return -1;
}

static void iterateMapListValues(linkedList* mapList, void (*iterator)(keyValPair*)) {
    for (node* current = mapList->head; current; current = current->next) {
        iterator(current->value);
    }
}

static void freeMapList(linkedList* l, bool freeKeysAndVals) {
    for (node* current = l->head; current; current = current->next) {
        keyValPair* kvpp = current->value;
        if (freeKeysAndVals) {
            free(kvpp->key);
            free(kvpp->val);
        }
        free(kvpp);
        free(current);
    }
    free(l);
}

// Factor representing the number of items currently held in the table as a proportion of total table size, past which the table will expand by GROWTH_FACTOR
#define LOAD_FACTOR 0.75
// Factor which table size will be multiplied by when it passes LOAD_FACTOR
#define GROWTH_FACTOR 2

#define a 214013
#define c 2531011
#define INITIAL_SIZE 1000

static void fatalError(char* msg) {
    printf("Fatal error: %s\n", msg);
    exit(1);
}

/**simple linear congruental generator for pseudo random numbers.
* uses microsoft visual's values for a and c.
* Good for even spread but not good for true random behaviour as almost perfectly periodic.
* @param pointer to integer to be hashed 
**/
static int generateRand(int seed) {
    return (int)(a * (unsigned int)seed + c);
}

unsigned int intHash(void* num) {
    return generateRand(*(int*)num);
}

static int stringLength(char* str) {
    int len = 0;
    for (char ch = *str; ch != '\0'; ch = *++str, len++)
        ;
    return len;
}

unsigned int strHash(void* str) {
    char* strCasted = (char*)str;
    unsigned int fullHash = 0;
    unsigned int previousCharHash = 7;
    int len = stringLength(strCasted);
    for (int i = 0; i < len; i++) {
        unsigned int hash = generateRand(strCasted[i]);
        fullHash += hash * previousCharHash;
        previousCharHash = hash;
    }
    return fullHash;
}

static int generateTableHash(unsigned int rawHash, int tableSize) {
    return rawHash % tableSize;
}

hashMap* createHashMap(unsigned int (*keyHashFunction)(void*), bool (*keyComparator)(void*, void*)) {
    hashMap* hm = malloc(sizeof(hashMap));
    linkedList** buckets = malloc(sizeof(linkedList*) * INITIAL_SIZE);
    for (int i = 0; i < INITIAL_SIZE; i++) {
        buckets[i] = NULL;
    }
    *hm = (hashMap) {
        INITIAL_SIZE,
        0,
        keyHashFunction,
        keyComparator,
        buckets
    };
    return hm;
}

static void rehashTable(hashMap* hm, linkedList** newTable, int newSize) {
    for (int i = 0; i < hm->_bucketCount; i++) {
        linkedList* bucket = hm->table[i];
        if (bucket) {
            for (keyValPair* removed = removeFirst(bucket); removed; removed = removeFirst(bucket)) {
                int newHash = generateTableHash(hm->_keyHashFunction(removed->key), newSize);
                linkedList* l = newTable[newHash];
                if (!l) {
                    l = newTable[newHash] = createList();
                }
                appendToList(l, removed);
            }
            free(bucket);
        }
    }
}

static void expandHashMap(hashMap* hm) {
    int newSize = hm->_bucketCount * GROWTH_FACTOR;
    linkedList** newTable = malloc(sizeof(linkedList*) * newSize);
    if (!newTable) {
        fatalError("Failed to expand hash map");
    }
    printf("Expanding hash map from %i to %i\n", hm->_bucketCount, newSize);
    rehashTable(hm, newTable, newSize);
    free(hm->table);
    hm->table = newTable;
    hm->_bucketCount = newSize;
}

static keyValPair* createKeyValuePair(void* key, void* value) {
    keyValPair* kvpp = malloc(sizeof(keyValPair));
    if (!kvpp) {
        fatalError("Could not create key value pair");
    }
    *kvpp = (keyValPair) { key, value };
    return kvpp;
}

void addToMap(hashMap* hm, void* key, void* value, bool freeOnOverwrite) {
    int index = generateTableHash(hm->_keyHashFunction(key), hm->_bucketCount);

    keyValPair* kvpp = createKeyValuePair(key, value);

    linkedList* l = hm->table[index];
    if (!l) {
        l = hm->table[index] = createList();
    } else {
        keyValPair* removedkvpp = removeForKey(l, kvpp, hm->_keyComparator);
        if (removedkvpp && freeOnOverwrite) {
            free(removedkvpp->key);
            free(removedkvpp->val);
            free(removedkvpp);
        }
    }
    appendToList(l, kvpp);

    float newItemCount = (float)++hm->_itemCount;
    if ((newItemCount / hm->_bucketCount) > LOAD_FACTOR) {
        expandHashMap(hm);
    }
}

/**
 * Removes the item from the table. Does not call free on the item removed.
 * @return pointer to the item if successful, NULL if table did not contain item.
 **/
keyValPair* removeKeyValPair(hashMap* hm, void* keyp) {
    int hashedInd = generateTableHash(hm->_keyHashFunction(keyp), hm->_bucketCount);
    linkedList* l = hm->table[hashedInd];
    if (l) {
        keyValPair* removed = removeForKey(l, keyp, hm->_keyComparator);
        if (removed) {
            hm->_itemCount--;
            return removed;
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

bool mapContainsKey(hashMap* hm, void* key) {
    int hashedInd = generateTableHash(hm->_keyHashFunction(key), hm->_bucketCount);
    linkedList* l = hm->table[hashedInd];
    if (l) {
        int index = findIndexForKey(l, key, hm->_keyComparator);
        return index != -1;
    } else {
        return false;
    }
}

void* getValueForKey(hashMap* hm, void* key) {
    int hashedInd = generateTableHash(hm->_keyHashFunction(key), hm->_bucketCount);
    linkedList* l = hm->table[hashedInd];
    if (l) {
        int index = findIndexForKey(l, key, hm->_keyComparator);
        if (index == -1) {
            return NULL;
        } else {
            keyValPair* kvpp = getValueAt(l, index);
            return kvpp->val;
        }
    } else {
        return NULL;
    }
}

void clearMap(hashMap* hm, bool freeKeysAndVals) {
    for (int i = 0; i < hm->_bucketCount; i++) {
        linkedList* bucket = hm->table[i];
        if (bucket) {
            freeMapList(bucket, freeKeysAndVals);
            hm->table[i] = NULL;
        }
    }
    hm->_itemCount = 0;
}

void freeMap(hashMap* hm, bool freeKeysAndVals) {
    clearMap(hm, freeKeysAndVals);
    free(hm->table);
    free(hm);
}

bool isEmptyMap(hashMap* hm) {
    return hm->_itemCount == 0;
}

void printMapPairs(hashMap* hm, void (*keyValPrinter)(keyValPair*)) {
    for (int i = 0; i < hm->_bucketCount; i++) {
        linkedList* bucket = hm->table[i];
        if (bucket) {
            iterateMapListValues(bucket, keyValPrinter);
        }
    }
}
