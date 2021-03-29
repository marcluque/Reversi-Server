//
// Created with <3 by marcluque, August 2020
//

#include "map_loader.h"

static const int MAX_LINE_LENGTH = 512;

char* map_loader_processMap(char* baseDir, const char* mapName) {
    char* infix = "/";
    char path[1 + strlen(baseDir) + strlen(infix) + strlen(mapName)];
    strcpy(path, dirname(baseDir));
    strcat(path, infix);
    strcat(path, mapName);
    FILE* file = fopen(path, "r");

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
    numberOfStones = malloc(sizeof(int) * (NUM_PLAYERS + 1));
    // TODO: COUNT HOW MANY STONES A PLAYER HAS

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
    numberOfOverride = malloc(sizeof(int) * (NUM_PLAYERS + 1));
    for (int i = 1; i <= NUM_PLAYERS; ++i) {
        numberOfOverride[i] = NUM_OVERRIDE;
    }

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
    numberOfBombs = malloc(sizeof(int) * (NUM_PLAYERS + 1));
    for (int i = 1; i <= NUM_PLAYERS; ++i) {
        numberOfBombs[i] = NUM_BOMBS;
    }

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
    map = (char**) malloc(MAP_HEIGHT * sizeof(char*));
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
                if (isTilePlayer(buffer[j])) {
                    numberOfStones[playerToInt(buffer[j])] += isTilePlayer(buffer[j]);
                }
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
        Transition transition1;
        transition1.x = (int) strtol(splitToDelimiter, NULL, 10);
        splitToDelimiter = strtok(NULL, delimiter);
        transition1.y = (int) strtol(splitToDelimiter, NULL, 10);
        splitToDelimiter = strtok(NULL, delimiter);
        transition1.direction = (int) strtol(splitToDelimiter, NULL, 10);
        splitToDelimiter = strtok(NULL, delimiter);
        // Ignore <->
        splitToDelimiter = strtok(NULL, delimiter);

        // Get end of transition
        Transition transition2;
        transition2.x = (int) strtol(splitToDelimiter, NULL, 10);
        splitToDelimiter = strtok(NULL, delimiter);
        transition2.y = (int) strtol(splitToDelimiter, NULL, 10);
        splitToDelimiter = strtok(NULL, delimiter);
        transition2.direction = (int) strtol(splitToDelimiter, NULL, 10);

        transitiontable_add(&transition1, &transition2);
        transitiontable_add(&transition2, &transition1);
    }

    // Store whole file in a string
    fseek(file, 0L, SEEK_SET);
    fseek(file, 0L, SEEK_END);
    long numberOfBytes = ftell(file);
    fseek(file, 0L, SEEK_SET);

    char* mapString = malloc(sizeof(char) * numberOfBytes);

    if (mapString == NULL) {
        fprintf(stdout, RED "Error while allocating memory for map\n" RESET);
        exit(1);
    }

    fread(mapString, sizeof(char), numberOfBytes, file);

    fclose(file);

    // Dont't free mapString
    return mapString;
}