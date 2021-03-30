//
// Created with <3 by marcluque, August 2020
//

#include "map.h"

//// Public variables
/////////////////////
struct ListHead capturableStonesHead = SLIST_HEAD_INITIALIZER(capturableStonesHead);
struct ListHead* capturableStonesHeadPointer = &capturableStonesHead;

struct ListHead pathHead = SLIST_HEAD_INITIALIZER(pathHead);
struct ListHead* pathHeadPointer = &pathHead;

char** map = NULL;
int* numberOfStones = NULL;
int* numberOfOverride = NULL;
int* numberOfBombs = NULL;

//// Private variables
//////////////////////
static const int CORNERS[8][2] = {{0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}};

//// Private functions
//////////////////////
static bool walkPath(int startX, int startY, int direction, char player, bool useList) {
    int x = startX;
    int y = startY;
    // Starts at -1 because the do while immediately adds the start tile, but the start tile doesn't count for a path
    int pathLength = -1;

    do {
        Transition* transitionEnd = transitiontable_get(&((Transition) {x, y, direction}));

        // Follow the transition, if there is one and adapt its direction
        if (transitionEnd != NULL) {
            // Jump to stone the transitions ends on
            x = transitionEnd->x;
            y = transitionEnd->y;
            direction = (transitionEnd->direction + 4) % 8;
        } else {
            // Move in the specified direction, while the next stone still is another player's stone
            x += CORNERS[direction][0];
            y += CORNERS[direction][1];
        }

        if (useList) {
            Item* coordinate = malloc(sizeof(Item));
            coordinate->x = x;
            coordinate->y = y;
            SLIST_INSERT_HEAD(pathHeadPointer, coordinate, nextItem);
        }

        pathLength++;
    } while (isCoordinateInMap(x, y) && isCapturableStone(map[y][x], player));

    // Check whether the last tile of the path is in the map, not the start tile and has the player stone on it
    return isCoordinateInMap(x, y)
           && pathLength > 0
           && map[y][x] == player
           && (startX != x || startY != y);
}

static void executeBuildMove(int x, int y, char player, int specialTile) {
    int playerId = playerToInt(player);

    if (isTileOccupied(map[y][x])) {
        numberOfOverride[playerId]--;
    }

    Item* item;
    SLIST_FOREACH(item, capturableStonesHeadPointer, nextItem) {
        // If a special tile is encountered, ignore, as it's dealt with separately
        if (isTileSpecial(map[item->y][item->x])) {
            numberOfStones[playerId]++;
            break;
        } else if (isTilePlayer(map[item->y][item->x])) {
            numberOfStones[playerToInt(map[item->y][item->x])]--;
        }

        numberOfStones[playerId]++;
        map[item->y][item->x] = player;
    }

    // Check whether field is a special field
    switch (map[y][x]) {
        case 'c':
            map[y][x] = player;

            // Switch stones of the players in numberOfStones array
            int temp = numberOfStones[specialTile];
            numberOfStones[specialTile] = numberOfStones[playerId];
            numberOfStones[playerId] = temp;

            // Switches stones of the player specified in choiceStonePlayer
            char playerWithMostStonesChar = intToPlayer(specialTile);
            for (int i = 0; i < MAP_HEIGHT; i++) {
                for (int j = 0; j < MAP_WIDTH; j++) {
                    if (map[i][j] == playerWithMostStonesChar) {
                        map[i][j] = player;
                    } else if (map[i][j] == player) {
                        map[i][j] = playerWithMostStonesChar;
                    }
                }
            }

            break;
        case 'i':
            map[y][x] = player;

            // Switches the stones of the players
            for (int i = 1; i <= NUM_PLAYERS; i++) {
                int nextPlayer = (i % NUM_PLAYERS) + 1;
                int tempNumber = numberOfStones[i];
                numberOfStones[i] = numberOfStones[nextPlayer];
                numberOfStones[nextPlayer] = tempNumber;
            }

            // Iterates over all players and switches their stones with (i % numberOfPlayers) + 1 where i is the player
            for (int i = 0; i < MAP_HEIGHT; i++) {
                for (int j = 0; j < MAP_WIDTH; j++) {
                    if (isTilePlayer(map[i][j])) {
                        map[i][j] = intToPlayer((playerToInt(map[i][j]) % NUM_PLAYERS) + 1);
                    }
                }
            }
            break;
        case 'b':
            map[y][x] = player;

            if (specialTile == 20) {
                numberOfBombs[playerId]++;
            } else {
                numberOfOverride[playerId]++;
            }
            break;
    }
}

static void executeBombMoveRecursive(int x, int y, int depth) {
    int startX = x;
    int startY = y;

    if (!isCoordinateInMap(x, y) || isTileHole(map[y][x])) {
        return;
    } else if (depth == BOMB_RADIUS) {
        map[y][x] = '$';
        return;
    }

    for (int i = 0; i < 8; ++i) {
        x = startX;
        y = startY;
        Transition* transitionEnd = transitiontable_get(&((Transition) {x, y, i}));

        if (transitionEnd != NULL) {
            x = transitionEnd->x;
            y = transitionEnd->y;
        } else {
            x += CORNERS[i][0];
            y += CORNERS[i][1];
        }

        if (isCoordinateInMap(x, y)) {
            executeBombMoveRecursive(x, y, depth + 1);
        }
    }

    x = startX;
    y = startY;
    map[y][x] = '$';
}

static void executeBombMove(int x, int y, char player) {
    numberOfBombs[playerToInt(player)]--;
    executeBombMoveRecursive(x, y, 0);

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            if (map[i][j] == '$') {
                map[i][j] = '-';
            }
        }
    }
}

//// Public functions
/////////////////////
bool map_getCapturableStones(int x, int y, char player, bool override, int phase) {
    return map_isMoveValid(x, y, player, false, override, true, phase);
}

bool map_isMoveValid(int x, int y, char player, bool returnEarly, bool override, bool useList, int phase) {
    if (isTileHole(map[y][x])) {
        return false;
    }

    // Building phase
    if (phase == 1) {
        // Tile may be occupied by player or expansion stone
        if (isTileOccupied(map[y][x]) && (!override || numberOfOverride[playerToInt(player)] == 0)) {
            return false;
        }

        // Used for allowing an override action without actually enclosing a path on an expansion stone!
        bool finalResult = isTileExpansion(map[y][x]);
        bool tempResult;

        if (useList) {
            // Add start stone to capturable stones
            Item* startCoordinate = malloc(sizeof(Item));
            startCoordinate->x = x;
            startCoordinate->y = y;
            SLIST_INSERT_HEAD(capturableStonesHeadPointer, startCoordinate, nextItem);
        }

        // Iterate over all directions from start tile
        for (int direction = 0; direction < 8; direction++) {
            // Walk along direction starting from (x,y)
            tempResult = walkPath(x, y, direction, player, useList);
            finalResult |= tempResult;

            if (returnEarly & finalResult) {
                return true;
            }

            if (useList) {
                while (!SLIST_EMPTY(pathHeadPointer)) {
                    Item* item = SLIST_FIRST(pathHeadPointer);
                    SLIST_REMOVE_HEAD(pathHeadPointer, nextItem);
                    if (tempResult) {
                        SLIST_INSERT_HEAD(capturableStonesHeadPointer, item, nextItem);
                    }
                }
            }
        }

        return finalResult;
    } else {
        // Elimination phase
        // Check basic invalidity for elimination phase
        return numberOfBombs[playerToInt(player)] > 0;
    }
}

void map_executeMove(int x, int y, char player, int specialTile, int phase) {
    if (phase == 1) {
        executeBuildMove(x, y, player, specialTile);
    } else {
        executeBombMove(x, y, player);
    }
}

void map_emptyCapturableStones() {
    while (!SLIST_EMPTY(capturableStonesHeadPointer)) {
        Item* item = SLIST_FIRST(capturableStonesHeadPointer);
        SLIST_REMOVE_HEAD(capturableStonesHeadPointer, nextItem);
        free(item);
    }
}

void map_cleanUp() {
    for (int i = 0; i < MAP_HEIGHT; ++i) {
        free(map[i]);
    }
    free(map);

    free(numberOfStones);
    free(numberOfBombs);
    free(numberOfOverride);
}