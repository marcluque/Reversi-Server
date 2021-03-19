#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
#pragma ide diagnostic ignored "hicpp-multiway-paths-covered"
//
// Created with <3 by marcluque, August 2020
//

#ifndef REVERSI_SERVER_MAP_UTIL_H
#define REVERSI_SERVER_MAP_UTIL_H

#include "../globals.h"

//// Singly Linked List
///////////////////////
// Define struct for head of list
SLIST_HEAD(ListHead, Item);
struct ListHead* capturableStonesHeadPointer;
struct Item {
    int x;
    int y;
    SLIST_ENTRY(Item) nextItem;
};

typedef struct Item Item;
typedef struct ListHead ListHead;

//// Transition hash table
//////////////////////////
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

extern TransitionPair* transitionPairs;

Transition* transitiontable_get(Transition* transition);

void transitiontable_add(Transition* transitionKey, Transition* value);

void transitiontable_cleanUp();

//// Game map variables
///////////////////////
int NUM_PLAYERS;
int NUM_OVERRIDE;
int NUM_BOMBS;
int BOMB_RADIUS;
int MAP_HEIGHT;
int MAP_WIDTH;

//// Game map util functions
////////////////////////////
static inline bool isTileHole(char tile) {
    return tile == '-';
}

static inline bool isTilePlayer(char tile) {
    return '1' <= tile && tile <= '8';
}

static inline bool isTileExpansion(char tile) {
    return tile == 'x';
}

// Considers tiles with expansion stones as occupied according to the specification.
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

//// Game map printing functions
////////////////////////////////
static inline void printMap(char** map) {
    for (int i = 0; i < MAP_HEIGHT; ++i) {
        for (int j = 0; j < MAP_WIDTH; ++j) {
             printf("%c ", map[i][j]);
        }
        printf("\n");
    }
}

static inline void printWithCapturedStonesMap(char** map, int startX, int startY) {
    bool captured = false;
    for (int i = 0; i < MAP_HEIGHT; ++i) {
        for (int j = 0; j < MAP_WIDTH; ++j) {
            if (j == startX && i == startY) {
                printf(BOLDBLUE "%c" RESET " ", map[i][j]);
                continue;
            }

            Item* item;
            SLIST_FOREACH(item, capturableStonesHeadPointer, nextItem) {
                if (item->x == j && item->y == i) {
                    captured = true;
                    break;
                }
            }

            if (captured) {
                printf(BOLDYELLOW "%c" RESET " ", map[i][j]);
                captured = false;
            } else {
                printf("%c ", map[i][j]);
            }
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