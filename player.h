/*
 * =====================================================================================
 *
 *       Filename:  player.h
 *
 *    Description:  player
 *
 *        Version:  1.0
 *        Created:  03/25/2016 12:59:29 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jignesh Darji (jignesh), jndarji@ncsu.edu
 *   Organization:  North Carolina State University
 *
 * =====================================================================================
 */

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "debug.h"
#include "socketlib.h"
#include "playerinfo.h"

//Event Handler
int registerEventHandlers();
int serverStartedEventHandler(int sockfd, struct sockaddr_in* leftSockAddr);
int masterConnectedEventHandler(int sockfd, struct sockaddr* masterSock);
int rightInfoReceivedHandler(int sockfd, char* host, int port);

//Communication
int sendLeftPortToMaster(int sockfd);
int sendRightACKToMaster(int sockfd);

#endif
