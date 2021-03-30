//
// Created with <3 by marcluque, August 2020
//

#pragma once

#include "../globals.h"
#include "../map/map.h"

void server_initServer(char* hostParam, char* portParam, int timeLimitParam, int depthLimitParam);

struct pollfd* server_acceptConnections();

void server_sendMapData(char* mapString);

void server_sendPlayerNumber();

int server_startPhase(int phase, int startingPlayer);

void server_cleanUp();