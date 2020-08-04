//
// Created with <3 by DataSecs on 03.08.20.
//

#ifndef REVERSI_SERVER_SERVER_H
#define REVERSI_SERVER_SERVER_H

#include "../globals.h"

extern struct SinglyLinkedList* headPointer;

void startServer(char* bindAddress, int port);

#endif //REVERSI_SERVER_SERVER_H
