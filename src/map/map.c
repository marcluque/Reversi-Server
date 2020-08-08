//
// Created with <3 by DataSecs on 03.08.20.
//

#include "map.h"

const int CORNERS[8][2] = {{0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}};

bool walkPath(int startX, int startY, int direction, char player) {
    int x = startX;
    int y = startY;
    // Starts at -1 because the do while immediately adds the start tile, but the start tile doesn't count for a path
    int pathLength = -1;

    do {
        Transition* transitionEnd = tableGetTransition(&((Transition) {x, y, direction}));

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

        pathLength++;
    } while (isCoordinateInMap(x, y) && isCapturableStone(map[y][x], player));

    // Check whether the last tile of the path is in the map, not the start tile and has the player stone on it
    return isCoordinateInMap(x, y)
           && pathLength > 0
           && map[y][x] == player
           && (startX != x || startY != y);
}

bool map_isMoveValid(int x, int y, char player, bool returnEarly, bool override, int phase) {
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
        bool result = isTileExpansion(map[y][x]);

        // Iterate over all directions from start tile
        for (int direction = 0; direction < 8; direction++) {
            // Walk along direction starting from (x,y)
            result |= walkPath(x, y, direction, player);

            if (returnEarly & result) {
                return true;
            }
        }

        return result;
    } else {
        // Elimination phase
        // Check basic invalidity for elimination phase
        return numberOfBombs[playerToInt(player)] > 0;
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