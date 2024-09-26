#include <stdio.h>

typedef enum {
    OBJ_INT,
    OBJ_PAIR
} ObjectType;

typedef struct sObject {
    ObjectType type;

    // because an object can either be an int OR a pair of objects, a union is used here to prevent memory
    // from being allocated unnecessarily. unions are structs where their fields overlap in memory.
    union {
        // OBJ_INT
        int value;

        // OBJ_PAIR
        struct {
            struct sObject* head;
            struct sObject* tail;
        };
    };
} Object;

int main(int argc, char* argv[]) {
    printf("Hello World!\n");
    return 0;
}
