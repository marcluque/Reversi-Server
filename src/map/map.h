//
// Created with <3 by marcluque, August 2020
//

#ifndef REVERSI_SERVER_MAP_H
#define REVERSI_SERVER_MAP_H

#include "../globals.h"
#include "map_util.h"

extern char** map;
extern int* numberOfStones;
extern int* numberOfOverride;
extern int* numberOfBombs;

bool map_isMoveValid(int x, int y, char player, bool returnEarly, bool override, bool useList, int phase);

bool map_getCapturableStones(int x, int y, char player, bool override, int phase);

void map_executeMove(int x, int y, char player, int specialTile, int phase);

void map_emptyCapturableStones();

void map_cleanUp();

#endif //REVERSI_SERVER_MAP_H