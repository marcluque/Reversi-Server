#include "globals.h"
#include "network/server.h"
#include "map/map.h"
#include "map/map_loader.h"

// Main
int main(int argc, char* argv[]) {
    char* mapName = NULL;
    char* hostAddress = "127.0.0.1";
    int port = 8080;
    double timeLimit = -1;
    int depthLimit = -1;

    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "-map") == 0 || strcmp(argv[i], "--map") == 0) {
            mapName = argv[i + 1];
        } else if (strcmp(argv[i], "-host") == 0 || strcmp(argv[i], "--host") == 0) {
            hostAddress = argv[i + 1];
        } else if (strcmp(argv[i], "-port") == 0 || strcmp(argv[i], "--port") == 0) {
            port = (int) strtol(argv[i + 1], NULL, 10);
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

    if (timeLimit == -1 && depthLimit == -1) {
        fprintf(stderr, RED "-timeLimit or -depthLimit is required!" RESET "\n");
        exit(1);
    } else if (timeLimit < 0.2) {
        fprintf(stderr, RED "timeLimit has to be >= 0.2!" RESET "\n");
        exit(1);
    }

    // Load map
    map = processMap(argv[0], mapName);

    // On start-up the server waits for the required number of players to join the server
    //startServer(hostAddress, port);

    return 0;
}
