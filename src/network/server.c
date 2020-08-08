#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
//
// Created with <3 by DataSecs on 03.08.20.
//

#include "server.h"
#include "../map/map.h"



//// Variables
//////////////
int fdCount;
struct pollfd* pollFDs;
char* host;
char* port;
int connectionLimit;
int timeLimit;
int depthLimit;



//// Private funtions
/////////////////////
void* getSocketAddress(struct sockaddr* socketAddress) {
    // Get socket adress in either IPv4 or IPv6
    if (socketAddress->sa_family == AF_INET) {
        return &(((struct sockaddr_in*) socketAddress)->sin_addr);
    } else {
        return &(((struct sockaddr_in6*) socketAddress)->sin6_addr);
    }
}

int getListenerSocket(int backlog) {
    // Listening socket descriptor
    int listener;
    // For setsockopt() SO_REUSEADDR
    int reUseAddresses = 1;

    /*
     * struct addrinfo {
     *      int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
     *      int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
     *      int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
     *      int              ai_protocol;  // use 0 for "any"
     *      size_t           ai_addrlen;   // size of ai_addr in bytes
     *      struct sockaddr *ai_addr;      // struct sockaddr_in or _in6
     *      char            *ai_canonname; // full canonical hostname
     *
     *      struct addrinfo *ai_next;      // linked list, next node
     *  };
     */
    struct addrinfo hints;
    struct addrinfo* addressList;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    //hints.ai_flags = AI_PASSIVE;

    int result = getaddrinfo(host, port, &hints, &addressList);
    if (result != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(result));
        exit(1);
    }

    struct addrinfo* address;
    for (address = addressList; address != NULL; address = address->ai_next) {
        listener = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
        if (listener < 0) {
            continue;
        }

        // Lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &reUseAddresses, sizeof(int));

        if (bind(listener, address->ai_addr, address->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    // If we got here, it means we didn't get bound
    if (address == NULL) {
        return -1;
    }

    freeaddrinfo(addressList);

    // Listen
    if (listen(listener, backlog) == -1) {
        return -1;
    }

    printf("Server listening on " CYAN "%s" RESET ":" CYAN "%s\n" RESET, host, port);
    fflush(stdout);

    return listener;
}

void addFD(int newFD) {
    pollFDs[fdCount].fd = newFD;
    pollFDs[fdCount].events = POLLIN;
    fdCount++;
}

void removeFDInConnectionPhase(int playerNumber) {
    // Copy the one from the end to the deleted one
    pollFDs[playerNumber] = pollFDs[fdCount - 1];

    fdCount--;
}

void removeFDInGamePhase(int playerNumber) {
    // Closes connection with player
    close(pollFDs[playerNumber].fd);
    pollFDs[playerNumber].fd = -1;
}

void sendMessage(void* message, size_t msgLength, char* errorType, int playerNumber) {
    if (send(pollFDs[playerNumber].fd, message, msgLength, 0) == -1) {
        printf(RED "Error while sending " BOLDRED "%s" RESET RED " to player " BLUE "%i!\n" RESET, errorType, playerNumber);
        printf(RED "Removing player " BLUE "%i" RED " from match!\n" RESET, playerNumber);
        fflush(stdout);
        close(pollFDs[playerNumber].fd);
        removeFDInGamePhase(playerNumber);
    }
}

void sendMoveRequest(int playerNumber) {
    // Send message type
    int8_t messageType = 4;
    sendMessage(&messageType, 1, "message type", playerNumber);

    // Send length
    int32_t messageSize = htonl(5);
    sendMessage(&messageSize, 4, "message size", playerNumber);

    // Send time limit
    int32_t messageTimeLimit = htonl(timeLimit);
    sendMessage(&messageTimeLimit, 4, "time limit", playerNumber);

    // Send depth limit
    int8_t messageDepthLimit = depthLimit;
    sendMessage(&messageDepthLimit, 1, "depth limit", playerNumber);
}

void sendDisqualification(int playerNumber) {
    removeFDInGamePhase(playerNumber);

    for (int i = 1; i <= fdCount; ++i) {
        if (pollFDs[i].fd == -1) {
            continue;
        }

        // Send message type
        int8_t messageType = 7;
        sendMessage(&messageType, 1, "message type", playerNumber);

        // Send length
        int32_t length = htonl(1);
        sendMessage(&length, 4, "message size", playerNumber);

        // Send length
        int8_t player = playerNumber;
        sendMessage(&player, 1, "player disqualification", playerNumber);
    }
}

void sendMoveAnnoucement(Coordinate* move, int specialField) {
    for (int playerNumber = 1; playerNumber <= fdCount; ++playerNumber) {
        if (pollFDs[playerNumber].fd == -1) {
            continue;
        }

        // Send message type
        int8_t messageType = 4;
        sendMessage(&messageType, 1, "message type", playerNumber);

        // Send length
        int32_t length = htonl(5);
        sendMessage(&length, 4, "message size", playerNumber);

        // Send x coordinate
        int16_t x = htons(move->x);
        sendMessage(&x, 2, "move x coordinate", playerNumber);

        // Send x coordinate
        int16_t y = htons(move->y);
        sendMessage(&y, 2, "move y coordinate", playerNumber);

        // Send x coordinate
        int8_t choice = specialField;
        sendMessage(&choice, 1, "special field", playerNumber);
    }
}

void sendPhaseAnnoucement(int phase) {
    for (int playerNumber = 1; playerNumber <= fdCount; ++playerNumber) {
        if (pollFDs[playerNumber].fd == -1) {
            continue;
        }

        // Send message type
        int8_t messageType = 7 + phase;
        sendMessage(&messageType, 1, "message type", playerNumber);
    }
}

void receiveMove() {
    // Check whether move is valid
    map_isMoveValid();

    // Apply move

    // Print some stuff for map

    // Send move to other players
    sendMoveAnnoucement();
}



//// Public functions
/////////////////////
void server_initServer(char *hostParam, char *portParam, int timeLimitParam, int depthLimitParam) {
    host = hostParam;
    port = portParam;
    timeLimit = timeLimitParam;
    depthLimit = depthLimitParam;
    connectionLimit = NUM_PLAYERS;
}

struct pollfd* server_acceptConnections() {
    // Listening socket descriptor
    int listener;

    // Newly accept()ed socket descriptor
    int newFD;

    /*
     * struct sockaddr_storage {
     *      sa_family_t  ss_family;     // address family
     *
     *      // all this is padding, implementation specific, ignore it:
     *      char      __ss_pad1[_SS_PAD1SIZE];
     *      int64_t   __ss_align;
     *      char      __ss_pad2[_SS_PAD2SIZE];
     * };
     *
     */
    // Client address
    struct sockaddr_storage remoteAddress;
    socklen_t addressLength;

    char remoteIP[INET6_ADDRSTRLEN];

    // Start off with space for connectionLimit + 1 (the +1 is for the listener socket)
    fdCount = 0;
    int fdSize = connectionLimit + 1;

    /*
     * struct pollfd {
     *      int fd;         // the socket descriptor
     *      short events;   // bitmap of events we're interested in
     *      short revents;  // when poll() returns, bitmap of events that occurred
     *  };
     */
    pollFDs = malloc(sizeof(*pollFDs) * fdSize);

    // Set up and get a listening socket
    listener = getListenerSocket(connectionLimit);

    if (listener == -1) {
        fprintf(stderr, RED "Error getting listening socket!" RESET "\n");
        exit(1);
    }

    // Add listener to set
    pollFDs[0].fd = listener;
    // Report ready to read on incoming connection
    pollFDs[0].events = POLLIN;
    // For the listener
    fdCount = 1;

    printf("Waiting for " BOLDGREEN "%i" RESET " clients to connect...\n", connectionLimit);
    fflush(stdout);

    // Let all players connect to the server
    while (fdCount != fdSize) {
        int pollCount = poll(pollFDs, fdCount, -1);

        if (pollCount == -1) {
            fprintf(stderr, RED "Error while polling!" RESET "\n");
            exit(1);
        }

        // Check if listener FD is ready to read
        if (pollFDs[0].revents & POLLIN) {
            addressLength = sizeof(remoteAddress);
            newFD = accept(listener, (struct sockaddr*) &remoteAddress, &addressLength);
            const void* socketAddress = getSocketAddress((struct sockaddr*) &remoteAddress);

            if (newFD == -1) {
                fprintf(stdout, RED "Error while accepting connection!" "\n");
                fprintf(stdout,
                        "Remote address:" CYAN "%s\n" RESET,
                        inet_ntop(remoteAddress.ss_family, socketAddress, remoteIP, INET6_ADDRSTRLEN));
            } else {
                printf("Player " BLUE "%i" RESET " connected with remote address " CYAN "%s\n" RESET,
                       fdCount,
                       inet_ntop(remoteAddress.ss_family, socketAddress, remoteIP, INET6_ADDRSTRLEN));

                addFD(newFD);
            }

            fflush(stdout);
        } else {
            // If not the listener, then it's a client. We only allow disconnects, otherwise ignore
            char message[256];
            for (int i = 1; i < fdSize; ++i) {
                if (pollFDs[i].revents & POLLIN && recv(pollFDs[i].fd, message, sizeof(message), 0) == 0) {
                    // Connection closed by client
                    printf("Player " BLUE "%i" RESET " closed the connection!\n", i);
                    fflush(stdout);
                    close(pollFDs[i].fd);
                    removeFDInConnectionPhase(i);
                }
            }
        }
    }

    return pollFDs;
}

void server_sendMapData(char* mapString) {
    // Send map to all connected clients
    for(int playerNumber = 1; playerNumber <= fdCount; playerNumber++) {
        if (pollFDs[playerNumber].fd == -1) {
            continue;
        }

        // Send message type
        int8_t messageType = 2;
        sendMessage(&messageType, 1, "message type", playerNumber);

        // Send length
        size_t length = strlen(mapString);
        int32_t mapSize = htonl(length);
        sendMessage(&mapSize, 4, "message size", playerNumber);

        // Send map
        sendMessage(mapString, length, "map", playerNumber);
    }
}

void server_sendPlayerNumber() {
    // Send respective player numbers to all connected clients
    for (int playerNumber = 1; playerNumber <= fdCount; playerNumber++) {
        if (pollFDs[playerNumber].fd == -1) {
            continue;
        }

        // Send message type
        int8_t messageType = 3;
        sendMessage(&messageType, 1, "message type", playerNumber);

        // Send length
        int32_t length = htonl(1);
        sendMessage(&length, 4, "message size", playerNumber);

        // Send player number
        int8_t player = playerNumber;
        sendMessage(&player, 1, "player number", playerNumber);
    }
}

void server_startGame() {
    while (true) {
        for (int playerNumber = 1; playerNumber <= fdCount; ++playerNumber) {
            if (pollFDs[playerNumber].fd == -1) {
                continue;
            }

            // Send move request to player
            sendMoveRequest(playerNumber);

            // Wait if time limit is not 0
            int waitingTime = timeLimit == 0 ? -1 : timeLimit;
            int pollCount = poll(pollFDs, fdCount, waitingTime);
            if (pollCount == -1) {
                fprintf(stderr, RED "Error while waiting for move of player " BLUE "%i" RESET "!\n");
            } else if (pollCount == 0) {
                // Player timed out while doing a move -> disqualify
                sendDisqualification(playerNumber)
            }

            // Player sent a move
            if (pollFDs[playerNumber].revents & POLLIN) {

            }
        }
    }
}

void server_cleanUp() {
    // Close connection with all clients
    for (int i = 1; i <= fdCount; ++i) {
        close(pollFDs[i].fd);
    }

    // Close listener socket
    close(pollFDs[0].fd);

    free(pollFDs);
}

#pragma clang diagnostic pop