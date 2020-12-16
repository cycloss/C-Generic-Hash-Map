#include "../src/hashMap.h"
#include <stdio.h>
#include <stdlib.h>

#define len(x) sizeof(x) / sizeof(x[0])

void keyValPrinter(keyValPair* kvpp) {
    printf("[%i : %s]\n", *(int*)kvpp->key, (char*)kvpp->val);
}

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

    printMapPairs(hm, keyValPrinter);
    freeMap(hm, false);
}