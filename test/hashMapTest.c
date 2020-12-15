#include "../src/hashMap.h"
#include "hashMapTest.h"
#include <stdio.h>
#include <stdlib.h>

int main() {

    hashMap* hm = createHashMap(intHash, intComp);
    int nums[80];
    char* msg = "hello";
    for (int i = 0; i < len(nums); i++) {
        nums[i] = i + 1;
        //TODO change to remove free on overwrite and just return a kvp
        addToMap(hm, &nums[i], msg, false);
    }

    int key = 80;
    if (mapContainsKey(hm, &key)) {
        puts("Has key");
        char* val = getValueForKey(hm, &key);
        printf("Val for key %d: %s\n", key, val);
    } else {
        puts("Doesn't have key");
    }

    // int rejectCount = 0;
    // for (int i = 0; i < len(nums); i++) {
    //     if (!addToMap(ht, &nums[i])) {
    //         rejectCount++;
    //     }
    // }
    // printf("Duplicate reject count: %d\n", rejectCount);

    // printIntTable(ht);
    // int itemToRemove = 5;
    // removeTableItem(ht, &itemToRemove) ? printf("Removed: %i\n", itemToRemove) : printf("Failed to remove %i\n", itemToRemove);

    // printIntTable(ht);
    // freeTable(ht, false);

    // hashMap* ht2 = createHashTable(strHash, stringComparator);
    // char* strs[] = { "a", "b", "aba", "aab" };

    // for (int i = 0; i < len(strs); i++) {
    //     addToMap(ht2, strs[i]);
    // }

    // printStrTable(ht2);
    // freeTable(ht2, false);
}

// void printIntItem(void* item) {
//     printf("%i, ", *(int*)item);
// }

// void printIntTable(hashMap* ht) {
//     iterateTableItems(ht, printIntItem);
//     puts("");
// }

// void printStrItem(void* item) {
//     printf("%s, ", (char*)item);
// }

// void printStrTable(hashMap* ht) {
//     iterateTableItems(ht, printStrItem);
//     puts("");
// }