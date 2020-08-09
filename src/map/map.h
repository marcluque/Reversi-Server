//
// Created with <3 by DataSecs on 03.08.20.
//

#ifndef REVERSI_SERVER_MAP_H
#define REVERSI_SERVER_MAP_H

#include "../globals.h"
#include "map_util.h"

char** map;
int* numberOfStones;
int* numberOfOverride;
int* numberOfBombs;

SLIST_HEAD(ListHead, Item) head = SLIST_HEAD_INITIALIZER(head);
struct ListHead* headPointer = &head;
struct Item {
    int x;
    int y;
    SLIST_ENTRY(Item) nextItem;
};

typedef struct Item Item;
typedef struct ListHead ListHead;

bool map_isMoveValid(int x, int y, char player, bool returnEarly, bool override, bool useList, int phase);

bool map_getCapturableStones(int x, int y, char player, bool override, int phase);

void map_executeMove(int x, int y, char player, int specialTile, int phase);

void map_emptyCapturableStones();

void map_cleanUp();

#endif //REVERSI_SERVER_MAP_H