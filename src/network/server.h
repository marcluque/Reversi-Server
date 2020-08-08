//
// Created with <3 by DataSecs on 03.08.20.
//

#ifndef REVERSI_SERVER_SERVER_H
#define REVERSI_SERVER_SERVER_H

#include "../globals.h"

void server_initServer(char *hostParam, char *portParam, int timeLimitParam, int depthLimitParam);

struct pollfd* server_acceptConnections();

void server_sendMapData(char* mapString);

void server_sendPlayerNumber();

void server_startGame();

void server_cleanUp();

#endif //REVERSI_SERVER_SERVER_H
