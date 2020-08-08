//
// Created with <3 by DataSecs on 03.08.20.
//

#ifndef REVERSI_SERVER_MAP_H
#define REVERSI_SERVER_MAP_H

#include "../globals.h"
#include "map_util.h"

typedef struct {
    int x;
    int y;
} Coordinate;

char** map;
int* numberOfStones;
int* numberOfOverride;
int* numberOfBombs;

bool map_isMoveValid(int x, int y, char player, bool returnEarly, bool override, int phase);

void map_cleanUp();

#endif //REVERSI_SERVER_MAP_H
