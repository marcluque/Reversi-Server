//
// Created with <3 by DataSecs on 03.08.20.
//

#ifndef REVERSI_SERVER_MAP_H
#define REVERSI_SERVER_MAP_H

#include "../globals.h"
#include "map_util.h"

extern char** map;
extern int* numberOfStones;
extern int* numberOfOverride;
extern int* numberOfBombs;

typedef struct Item {
    int x;
    int y;
    SLIST_ENTRY(Item) next;
} Item;

struct SinglyLinkedListHead {
	struct Item* slh_first;
};

#endif //REVERSI_SERVER_MAP_H
