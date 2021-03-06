/*
 * =====================================================================================
 *
 *       Filename:  master.h
 *
 *    Description:  Ring Master
 *
 *        Version:  1.0
 *        Created:  03/15/2016 09:16:23 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jignesh Darji (jignesh), jndarji@ncsu.edu
 *   Organization:  North Carolina State University
 *
 * =====================================================================================
 */

#ifndef __MASTER_H__
#define __MASTER_H__

#include "socketlib.h"
#include "playerinfo.h"
#include "debug.h"
#include "potato_protocol.h"

// Handlers
int playerConnectedEventHandler(int sockfd, struct sockaddr_in *playerSock);
int leftPortReceivedHandler(int sockfd, int port);
int rightACKReceivedHandler(int sockfd);
int potatoReceivedHandler(int sockfd, int hopsLeft, char* path);
int registerEventHandlers();

// Events
int allPlayersConnectedEvent();
int ringCompleteEvent();

// Communication
int sendRightNeighborInfoToAll();
int sendIDToPlayer(int playerID);
int sendPotato();

//MISC
int getPlayerIDFromSockFD(int sockfd);
int shutdownAllPlayers();
int shutdownSockets();
int shutdownListenerThreads();

#endif /* __MASTER_H__ */
