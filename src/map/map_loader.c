//
// Created with <3 by DataSecs on 05.08.20.
//

#include "map_loader.h"

const int MAX_LINE_LENGTH = 512;

char** processMap(char* baseDir, const char* mapName) {
    char* infix = "/";
    char* path = malloc(1 + strlen(baseDir) + strlen(infix) + strlen(mapName));
    strcpy(path, dirname(baseDir));
    strcat(path, infix);
    strcat(path, mapName);
    FILE* file = fopen(path, "r");
    free(path);

    if (file == NULL) {
        fprintf(stderr, RED "Couldn't open file %s!" RESET "\n", mapName);
        exit(1);
    }

    // Players
    char buffer[MAX_LINE_LENGTH];
    fgets(buffer, MAX_LINE_LENGTH, file);
    if (buffer[0] < 48 || buffer[0] > 56) {
        fprintf(stderr, RED "Player number has to be between 2 and 8" RESET "\n");
        exit(1);
    }
    NUM_PLAYERS = playerToInt(buffer[0]);

    // Override stones
    fgets(buffer, sizeof(buffer), file);
    int counter = 0;
    for (; buffer[counter] != '\n'; counter++);
    // +1 for NUL Termination
    counter++;
    char overrides[counter];
    memcpy(overrides, &buffer, counter);
    overrides[counter] = '\0';
    NUM_OVERRIDE = (int) strtol(overrides, NULL, 10);

    // Bombs amount
    fgets(buffer, sizeof(buffer), file);
    counter = 0;
    for (; buffer[counter] != ' '; counter++);
    // +1 for NUL Termination
    counter++;
    char bombs[counter];
    memcpy(bombs, &buffer, counter);
    bombs[counter] = '\0';
    NUM_BOMBS = (int) strtol(bombs, NULL, 10);

    // Bombs radius
    int newCounter = counter;
    for (; buffer[counter] != '\n'; counter++);
    // +1 for NUL Termination
    newCounter = counter - newCounter + 1;
    char bombRadius[newCounter];
    memcpy(bombRadius, &buffer[counter - 1], newCounter);
    bombRadius[newCounter] = '\0';
    BOMB_RADIUS = (int) strtol(bombRadius, NULL, 10);

    // Height
    fgets(buffer, sizeof(buffer), file);
    counter = 0;
    for (; buffer[counter] != ' '; counter++);
    // +1 for NUL Termination
    counter++;
    char height[counter];
    memcpy(height, &buffer, counter);
    height[counter] = '\0';
    MAP_HEIGHT = (int) strtol(height, NULL, 10);

    // Width
    newCounter = counter;
    for (; buffer[counter] != '\n'; counter++);
    // +1 for NUL Termination
    newCounter = counter - newCounter;
    counter -= newCounter;
    char width[newCounter];
    memcpy(width, &buffer[counter], newCounter);
    width[newCounter] = '\0';
    MAP_WIDTH = (int) strtol(width, NULL, 10);

    // Game map
    char** map = (char**) malloc(MAP_HEIGHT * sizeof(char*));
    for (int i = 0; i < MAP_HEIGHT; i++) {
        map[i] = (char*) malloc(MAP_WIDTH * sizeof(char));
    }

    for (int i = 0; i < MAP_HEIGHT; ++i) {
        fgets(buffer, sizeof(buffer), file);
        int colCount = 0;
        for (int j = 0; j < MAX_LINE_LENGTH; ++j) {
            if (buffer[j] == '\n') {
                break;
            } else if (buffer[j] != ' ') {
                map[i][colCount] = buffer[j];
                colCount++;
            }
        }
    }

    // Transitions
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        char* delimiter = " ";
        char* splitToDelimiter = strtok(buffer, delimiter);

        // Get start of transition
        Transition key;
        key.x = (int) strtol(splitToDelimiter, NULL, 10);
        splitToDelimiter = strtok(NULL, delimiter);
        key.y = (int) strtol(splitToDelimiter, NULL, 10);
        splitToDelimiter = strtok(NULL, delimiter);
        key.direction = (int) strtol(splitToDelimiter, NULL, 10);
        splitToDelimiter = strtok(NULL, delimiter);
        // Ignore <->
        splitToDelimiter = strtok(NULL, delimiter);

        // Get end of transition
        Transition value;
        value.x = (int) strtol(splitToDelimiter, NULL, 10);
        splitToDelimiter = strtok(NULL, delimiter);
        value.y = (int) strtol(splitToDelimiter, NULL, 10);
        splitToDelimiter = strtok(NULL, delimiter);
        value.direction = (int) strtol(splitToDelimiter, NULL, 10);

        tableAddTransitionPair(&key, &value);
    }

    fclose(file);

    return map;
}