//
// Created with <3 by DataSecs on 03.08.20.
//

#include "server.h"

// Get sockaddr, IPv4 or IPv6:
void* getSocketAddress(struct sockaddr* socketAddress) {
    if (socketAddress->sa_family == AF_INET) {
        return &(((struct sockaddr_in*) socketAddress)->sin_addr);
    } else {
        return &(((struct sockaddr_in6*) socketAddress)->sin6_addr);
    }
}

// Return a listener socket
int getListenerSocket(void) {
    // Listening socket descriptor
    int listener;
    // For setsockopt() SO_REUSEADDR, below
    int yes = 1;
    int rv;

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

    // Get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &addressList)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for (struct addrinfo* address = addressList; address != NULL; address = address->ai_next) {
        listener = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
        if (listener < 0) {
            continue;
        }

        // Lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, address->ai_addr, address->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    // If we got here, it means we didn't get bound
    if (p == NULL) {
        return -1;
    }

    freeaddrinfo(addressList); // All done with this

    // Listen
    if (listen(listener, 10) == -1) {
        return -1;
    }

    return listener;
}

// Add a new file descriptor to the set
void addFD(struct pollfd** pollFDs, int newFD, int* fdCount) {
    (*pollFDs)[*fdCount].fd = newFD;
    (*pollFDs)[*fdCount].events = POLLIN;
    (*fdCount)++;
}

// Remove an index from the set
void del_from_pfds(struct pollfd pfds[], int i, int *fd_count)
{
    // Copy the one from the end over this one
    pfds[i] = pfds[*fd_count-1];

    (*fd_count)--;
}

void sendMap() {

}

void startServer(char* bindAddress, int port) {
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

    // Buffer for client data
    char buf[256];

    char remoteIP[INET6_ADDRSTRLEN];

    // Start off with space for 8 connections
    int fdCount = 0;
    int fdSize = 9;

    /*
     * struct pollfd {
     *      int fd;         // the socket descriptor
     *      short events;   // bitmap of events we're interested in
     *      short revents;  // when poll() returns, bitmap of events that occurred
     *  };
     */
    struct pollfd* pollFDs = malloc(sizeof(*pollFDs) * fdSize);

    // Set up and get a listening socket
    listener = getListenerSocket();

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

    // 1. LET ALL PLAYERS CONNECT TO THE SERVER
    while (fdCount != 9) {
        int pollCount = poll(pollFDs, fdCount, -1);

        if (pollCount == -1) {
            fprintf(stderr, RED "Error while polling!" RESET "\n");
            exit(1);
        }

        // Run through the existing connections looking for data to read
        for(int i = 0; i < fdCount; i++) {
            // Check if FD is ready to read
            if (pollFDs[i].revents & POLLIN) {

                // If listener is ready to read, handle new connection
                if (pollFDs[i].fd == listener) {
                    addressLength = sizeof(remoteAddress);
                    newFD = accept(listener, (struct sockaddr*) &remoteAddress, &addressLength);
                    const void* socketAddress = getSocketAddress((struct sockaddr*) &remoteAddress);

                    if (newFD == -1) {
                        fprintf(stderr, RED "Error while accepting connection!" "\n");
                        fprintf(stderr,
                                "Remote address:" BLUE "%s\n" RESET,
                                inet_ntop(remoteAddress.ss_family, socketAddress, remoteIP, INET6_ADDRSTRLEN));
                    } else {
                        printf(GREEN "Player %i connected with address " BLUE "%s" RESET,
                               fdCount,
                               inet_ntop(remoteAddress.ss_family, socketAddress, remoteIP, INET6_ADDRSTRLEN));

                        // Send map and player number


                        addFD(&pollFDs, newFD, &fdCount);
                    }
                }
            } // END got ready-to-read from poll()
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!
}

void handleData() {
    if (pollFDs[i].fd == listener) {

    } else {
            // If not the listener, we're just a regular client
            int nbytes = recv(pollFDs[i].fd, buf, sizeof buf, 0);

            int sender_fd = pollFDs[i].fd;

            if (nbytes <= 0) {
                // Got error or connection closed by client
                if (nbytes == 0) {
                    // Connection closed
                    printf("pollserver: socket %d hung up\n", sender_fd);
                } else {
                    perror("recv");
                }

                close(pollFDs[i].fd); // Bye!

                del_from_pfds(pollFDs, i, &fdCount);

            } else {
                // We got some good data from a client

                for(int j = 0; j < fdCount; j++) {
                    // Send to everyone!
                    int dest_fd = pollFDs[j].fd;

                    // Except the listener and ourselves
                    if (dest_fd != listener && dest_fd != sender_fd) {
                        if (send(dest_fd, buf, nbytes, 0) == -1) {
                            perror("send");
                        }
                    }
                }
            }
        }
}

void receiveMove() {
}