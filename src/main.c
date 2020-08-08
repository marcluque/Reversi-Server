#include <zconf.h>
#include "globals.h"
#include "network/server.h"
#include "map/map.h"
#include "map/map_loader.h"

// Main
int main(int argc, char* argv[]) {
    char* mapName = NULL;
    char* host = "127.0.0.1";
    char* port = "8080";
    int timeLimit = 0;
    int depthLimit = 0;

    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "-map") == 0 || strcmp(argv[i], "--map") == 0) {
            mapName = argv[i + 1];
        } else if (strcmp(argv[i], "-host") == 0 || strcmp(argv[i], "--host") == 0) {
            host = argv[i + 1];
        } else if (strcmp(argv[i], "-port") == 0 || strcmp(argv[i], "--port") == 0) {
            port = argv[i + 1];
        } else if (strcmp(argv[i], "-timeLimit") == 0 || strcmp(argv[i], "--timeLimit") == 0) {
            timeLimit = (int) strtod(argv[i + 1], NULL);
        } else if (strcmp(argv[i], "-depthLimit") == 0 || strcmp(argv[i], "--depthLimit") == 0) {
            depthLimit = (int) strtol(argv[i + 1], NULL, 10);
        }
    }

    // Check validity of input
    if (mapName == NULL) {
        fprintf(stderr, RED "-map is required!" RESET "\n");
        exit(1);
    }

    if (timeLimit == 0 && depthLimit == 0) {
        fprintf(stderr, RED "-timeLimit or -depthLimit is required!" RESET "\n");
        exit(1);
    } else if (timeLimit < 0.2) {
        fprintf(stderr, RED "timeLimit has to be >= 0.2!" RESET "\n");
        exit(1);
    }

    // Calculate timeLimit from seconds to milliseconds
    timeLimit *= 1000;

    // Load map
    char* mapString = map_loader_processMap(argv[0], mapName);
    printf("Successfully parsed map!\n");
    fflush(stdout);

    // On start-up the server waits for the required number of players to join the server
    //server_startServer(host, port);

    server_initServer(host, port, timeLimit, depthLimit);
    server_acceptConnections();

    server_sendMapData(mapString);
    server_sendPlayerNumber();

    server_startGame();

    map_cleanUp();
    server_cleanUp();

    while (1);

    return 0;
}