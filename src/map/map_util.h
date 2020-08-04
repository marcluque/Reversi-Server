#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
#pragma ide diagnostic ignored "hicpp-multiway-paths-covered"
//
// Created with <3 by DataSecs on 04.08.20.
//

#ifndef REVERSI_SERVER_MAP_UTIL_H
#define REVERSI_SERVER_MAP_UTIL_H

#include "../globals.h"

typedef struct {
    int x;
    int y;
    int direction;
} Transition;

typedef struct TransitionPair {
    Transition* key;
    Transition* value;
    UT_hash_handle hh;
} TransitionPair;

extern const int NUM_PLAYERS;
extern const int NUM_OVERRIDE;
extern const int NUM_BOMBS;
extern const int BOMB_RADIUS;
extern const int MAP_HEIGHT;
extern const int MAP_WIDTH;

extern TransitionPair* transitionPairs;

extern inline void initTransitionTable() {
    transitionPairs = NULL;
}

extern inline Transition* getTransition(const Transition* transition) {
    TransitionPair* result;
    HASH_FIND_INT(transitionPairs, transition, result);
    return result->value;
}

extern inline void addTransitionPair(Transition* key, Transition* value) {
    TransitionPair* transitionPair = malloc(sizeof(TransitionPair));
    transitionPair->key = key;
    transitionPair->value = value;
    HASH_ADD_INT(transitionPairs, key, transitionPair);
}

extern inline bool isTileHole(char tile) {
    return tile == '-';
}

extern inline bool isTilePlayer(char tile) {
    return '1' <= tile && tile <= '8';
}

extern inline bool isTileExpansion(char tile) {
    return tile == 'x';
}

/**
 * Considers tiles with expansion stones as occupied according to the specification.
 *
 * @param tile
 * @return whether tile is expansion stone OR character from 1 to 8, using ascii values.
 */
extern inline bool isTileOccupied(char tile) {
    return isTileExpansion(tile) | isTilePlayer(tile);
}

extern inline bool isCoordinateInMap(int x, int y) {
    return x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT;
}

extern inline bool isCapturableStone(char** map, int x, int y, int player) {
    return isTileOccupied(map[y][x]) && map[y][x] != player;
}

extern inline bool isTileSpecial(char tile) {
    return tile == 'c' || tile == 'b' || tile == 'i';
}

extern inline int playerToInt(char player) {
    return player - 96;
}

#endif //REVERSI_SERVER_MAP_UTIL_H
#pragma clang diagnostic pop