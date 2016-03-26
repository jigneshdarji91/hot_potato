/*
 * =====================================================================================
 *
 *       Filename:  potato_protocol.h
 *
 *    Description:  Potato Protocol 
 *
 *        Version:  1.0
 *        Created:  03/25/2016 05:58:43 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jignesh Darji (jignesh), jndarji@ncsu.edu
 *   Organization:  North Carolina State University
 *
 * =====================================================================================
 */

#ifndef __POTATO_PROTOCOL_H__
#define __POTATO_PROTOCOL_H__

#include "debug.h"

int createLeftSocketPortMessage(int port, char* message);
int createPlayerIDMessage(int playerID, int leftNeighborID, int rightNeighborID, char* message);
int createPotatoMessage(int noOfHops, char* path, char* message);
int createRightNeighborInfoMessage(char* host, char* port, char* message);
int createRighACKReceivedMessage(char* message);

//Parse Control Messages
int parseMessage(int sockfd, char* message);
int parseMessageLeftPortOnMaster(int sockfd, char* message);
int parseMessageRightConnectedOnMaster(int sockfd, char* message);
int parseMessagePlayerIDOnPlayer(int sockfd, char* message);
int parseMessageRightInfoOnPlayer(int sockfd, char* message);
int parseShutdown(int sockfd);

//Parse Potato
int parsePotato(int sockfd, char* message);


//Callbacks

int registerLeftPortReceivedOnMasterCallback(void *cb);
int registerRightACKReceivedOnMasterCallback(void *cb);
int registerPlayerIDReceivedOnPlayerCallback(void *cb);
int registerRightInfoReceivedOnPlayerCallback(void *cb);
int registerServerConnectedCallback(void *cb);
int registerServerStartedCallback(void *cb);
int registerPotatoReceivedCallback(void *cb);

#endif
