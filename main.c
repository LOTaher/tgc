#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define STACK_MAX 256

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

// a virtual machine to have a stack that stores the variables in the current scope
typedef struct {
    // the max amount of objects that can be in the stack
    Object* stack[STACK_MAX];
    int stackSize;
} VM;

VM* newVM() {
    VM* vm = (VM*) malloc(sizeof(VM));
    vm->stackSize = 0;
    return vm;
}

void push(VM* vm, Object* value) {
    // stack overflow
    assert(vm->stackSize < STACK_MAX);
    // set the pushed object to the highest level of the stack
    // the first push would be index 0
    vm->stack[vm->stackSize] = value;
    // increase the size of the stack
    vm->stackSize++;
}

Object* pop(VM* vm) {
    // stack underflow
    assert(vm->stackSize > 0);
    // decrease the size of the stack
    vm->stackSize--;
    // return the object
    Object* value = vm->stack[vm->stackSize];
    // clear the index of the returned value
    vm->stack[vm->stackSize] = NULL;
    return value;
}

int main(int argc, char* argv[]) {
    printf("Hello World!\n");
    return 0;
}
