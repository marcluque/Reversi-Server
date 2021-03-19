//
// Created with <3 by marcluque, September 2020
//

#include "map_util.h"

TransitionPair* transitionPairs = NULL;

Transition* transitiontable_get(Transition* transition) {
    TransitionPair* result;
    HASH_FIND(hh, transitionPairs, transition, sizeof(Transition), result);
    return result == NULL ? NULL : &(result->value);
}

void transitiontable_add(Transition* transitionKey, Transition* value) {
    TransitionPair* transitionPair = malloc(sizeof(TransitionPair));
    transitionPair->key = *transitionKey;
    transitionPair->value = *value;
    HASH_ADD(hh, transitionPairs, key, sizeof(Transition), transitionPair);
}

void transitiontable_cleanUp() {
    TransitionPair* result;
    TransitionPair* tmp;
    HASH_ITER(hh, transitionPairs, result, tmp) {
        HASH_DEL(transitionPairs, result);
        free(result);
    }
}