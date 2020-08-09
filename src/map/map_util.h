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

typedef struct {
    Transition key;
    Transition value;
    UT_hash_handle hh;
} TransitionPair;

int NUM_PLAYERS;
int NUM_OVERRIDE;
int NUM_BOMBS;
int BOMB_RADIUS;
int MAP_HEIGHT;
int MAP_WIDTH;

static TransitionPair* transitionPairs = NULL;

static inline Transition* tableGetTransition(Transition* transition) {
    TransitionPair* result;
    HASH_FIND_INT(transitionPairs, transition, result);
    return result == NULL ? NULL : &(result->value);
}

static inline void tableAddTransitionPair(Transition* transitionKey, Transition* value) {
    TransitionPair* transitionPair = malloc(sizeof(TransitionPair));
    transitionPair->key = *transitionKey;
    transitionPair->value = *value;
    HASH_ADD_INT(transitionPairs, key, transitionPair);
}

static inline bool isTileHole(char tile) {
    return tile == '-';
}

static inline bool isTilePlayer(char tile) {
    return '1' <= tile && tile <= '8';
}

static inline bool isTileExpansion(char tile) {
    return tile == 'x';
}

/*
 * Considers tiles with expansion stones as occupied according to the specification.
 *
 * @param tile
 * @return whether tile is expansion stone OR character from 1 to 8, using ascii values.
 */
static inline bool isTileOccupied(char tile) {
    return isTileExpansion(tile) | isTilePlayer(tile);
}

static inline bool isCoordinateInMap(int x, int y) {
    return x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT;
}

static inline bool isCapturableStone(char tile, int player) {
    return isTileOccupied(tile) && tile != player;
}

static inline bool isTileSpecial(char tile) {
    return tile == 'c' || tile == 'b' || tile == 'i';
}

static inline int playerToInt(char player) {
    return player - '0';
}

static inline char intToPlayer(int player) {
    return (char) ('0' + player);
}

static inline void printMap(char** map) {
    for (int i = 0; i < MAP_HEIGHT; ++i) {
        for (int j = 0; j < MAP_WIDTH; ++j) {
             printf("%c ", map[i][j]);
        }
        printf("\n");
    }
}

static inline void printNiceMap(char** map, char playerAtTurn) {
    for (int i = 0; i < MAP_HEIGHT; ++i) {
        for (int j = 0; j < MAP_WIDTH; ++j) {
            // TODO: Print ' to mark as possible move and cyan

            switch (map[i][j]) {
                case 'c':
                    printf(YELLOW "c " RESET);
                    break;
                case 'i':
                    printf(MAGENTA "i " RESET);
                    break;
                case 'b':
                    printf(GREEN "b " RESET);
                    break;
                case '-':
                    printf(BLACK "- " RESET);
                    break;
                default:
                    if (map[i][j] == playerAtTurn) {
                        printf(CYAN "%c " RESET, map[i][j]);
                    } else {
                        printf("%c ", map[i][j]);
                    }
            }

            printf("%c ", map[i][j]);
        }
        printf("\n");
    }
}

#endif //REVERSI_SERVER_MAP_UTIL_H