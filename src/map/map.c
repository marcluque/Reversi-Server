//
// Created with <3 by DataSecs on 03.08.20.
//

#include "map.h"

bool walkPath(int startX, int startY, int direction, char player, struct SinglyLinkedListHead* head) {
    SLIST_INSERT_HEAD(head, (struct Item*) malloc(sizeof(struct Item)), entries);

    int x = startX;
    int y = startY;
    bool firstTile = true;

    // Check for placed stone whether there are transitions
    Transition* transitionEnd = NULL; /* Get transition from hash table */
    if (transitionEnd != NULL) {
        firstTile = false;
        if (!isCapturableStone(map, transitionEnd->x, transitionEnd->y, player)) {
            return false;
        }

        x = transitionEnd->x;
        y = transitionEnd->y;
        direction = (transitionEnd->direction + 4) % 8;
    }

    int pathLength = 0;
    while (firstTile || (isCoordinateInMap(x, y) && isCapturableStone(map, x, y, player))) {
        Item* coordinate = malloc(sizeof(Item));
        coordinate->x = x;
        coordinate->y = y;
        SLIST_INSERT_HEAD(head, coordinate, next);

        Transition t = {t.x = x, t.y = y, t.direction = direction};
        transitionEnd = NULL; /* Get from hash table */

        // Follow the transition, if there is one and adapt its direction
        if (transitionEnd != null) {
            // Jump to stone the transitions ends on
            x = transitionEnd.getX();
            y = transitionEnd.getY();
            direction = (transitionEnd.getR() + 4) % 8;
        } else {
            // Move in the specified direction, while the next stone still is another player's stone
            x += CORNERS[direction][0];
            y += CORNERS[direction][1];
        }

        pathLength += firstTile ? 0 : 1;
        firstTile = false;
    }

    // Check whether the last tile of the path is in the map, not the start tile and has the player stone on it
    return isCoordinateInMap(x, y)
           && pathLength > 0
           && map[y][x] == player
           && (startX != x || startY != y);
}

bool isMoveValid(int x, int y, char player, bool returnEarly, bool override, int phase) {
    if (isTileHole(map[y][x])) {
        return false;
    }

    // Building phase
    if (phase == 1) {
        // Used for allowing an override action without actually enclosing a path on an expansion stone!
        bool expansionStoneAllowed = false;

        // Check basic invalidity
        if (isTileOccupied(map[y][x])) {
            // If there are overrideStones, those may be used to do a move that can later enclose a path,
            // therefore no return is done, just if the stones are used up
            if (!override || numberOfOverride[playerToInt(player)] == 0) {
                return false;
            }

            // Just true, if the stone is an expansion stone, otherwise the expansion rule doesn't apply
            expansionStoneAllowed = isTileExpansion(map[y][x]);
        }

        SLIST_HEAD(SinglyLinkedListHead, Item) head = SLIST_HEAD_INITIALIZER(head);
        SLIST_INIT(&head);

        bool result = expansionStoneAllowed;

        // Iterate over all directions from start stone
        for (int direction = 0; direction < 8; direction++) {
            // Walk along direction starting from (x,y)
            result |= walkPath(x, y, direction, player, &head);

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