#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define STACK_MAX 256
#define INITIAL_GC_THRESHOLD 8

typedef enum { OBJ_INT, OBJ_PAIR } ObjectType;

typedef struct sObject {
  ObjectType type;

  // because an object can either be an int OR a pair of objects, a union is
  // used here to prevent memory from being allocated unnecessarily. unions are
  // structs where their fields overlap in memory.
  union {
    // OBJ_INT
    int value;

    // OBJ_PAIR
    struct {
      struct sObject *head;
      struct sObject *tail;
    };
  };

  unsigned char marked;

  // the next object in the list of all the objects. this allows for their to be
  // a global list of all objects in the program
  struct sObject *next;
} Object;

// a virtual machine to have a stack that stores the variables in the current
// scope
typedef struct {
  // the max amount of objects that can be in the stack
  Object *stack[STACK_MAX];
  int stackSize;
  // the first object in the list of all the objects
  Object *firstObject;

  // the total number of allocated objects
  int numObjects;

  // the number of objects required to trigger a gc
  int maxObjects;
} VM;

VM *newVM() {
  VM *vm = (VM *)malloc(sizeof(VM));
  vm->stackSize = 0;
  vm->firstObject = NULL;
  vm->maxObjects = INITIAL_GC_THRESHOLD;
  return vm;
}

void push(VM *vm, Object *value) {
  // stack overflow
  assert(vm->stackSize < STACK_MAX);
  // set the pushed object to the highest level of the stack
  // the first push would be index 0
  vm->stack[vm->stackSize] = value;
  // increase the size of the stack
  vm->stackSize++;
}

Object *pop(VM *vm) {
  // stack underflow
  assert(vm->stackSize > 0);
  // decrease the size of the stack
  vm->stackSize--;
  // return the object
  Object *value = vm->stack[vm->stackSize];
  // clear the index of the returned value
  vm->stack[vm->stackSize] = NULL;
  return value;
}

// create a new object to test garbage collection
Object *newObject(VM *vm, ObjectType type) {
  Object *object = (Object *)malloc(sizeof(Object));
  object->type = type;
  object->marked = 0;
  vm->numObjects++;

  // insert the object in the list of allocated objects
  object->next = vm->firstObject;
  vm->firstObject = object;

  return object;
}

// without an interpreter, there needs to be a manual way to push objects onto
// the stack
void pushInt(VM *vm, int intValue) {
  Object *object = newObject(vm, OBJ_INT);
  object->value = intValue;
  push(vm, object);
}

Object *pushPair(VM *vm) {
  Object *object = newObject(vm, OBJ_PAIR);
  // the object head and tail are popped because of the LIFO manner of the stack
  // instead of being two individual objects, they need to be popped to become
  // one pair object
  object->head = pop(vm);
  object->tail = pop(vm);

  push(vm, object);
  return object;
}

void mark(Object *object) {
  // if the current object is already marked, we're done. check this first to
  // avoid recursing on cycles in the object graph.
  if (object->marked == 1) {
    return;
  }

  object->marked = 1;

  if (object->type == OBJ_PAIR) {
    mark(object->head);
    mark(object->tail);
  }
}

// marks all the objects in the stack
void markAll(VM *vm) {
  for (int i; i < vm->stackSize; i++) {
    mark(vm->stack[i]);
  }
}

void sweep(VM *vm) {
  Object **object = &vm->firstObject;
  while (*object) {
    if (!(*object)->marked) {
      // if the object wasn't reached, remove it from the list and free it.
      Object *unreached = *object;

      *object = unreached->next;
      free(unreached);
      vm->numObjects--;
    } else {
      // this object was reached, so unmark it (for the next GC) and move on to
      // the next object
      (*object)->marked = 0;
      object = &(*object)->next;
    }
  }
}

void gc(VM *vm) {
  markAll(vm);
  sweep(vm);

  // the mulitplier lets the heap grow as the number of living objects
  // increases. likewise, it will shrink automatically if a bunch of objects end
  // up being freed
  vm->maxObjects = vm->numObjects * 2;
}

int main(int argc, char *argv[]) {
  printf("Hello World!\n");
  return 0;
}
