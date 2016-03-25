/*
 * =====================================================================================
 *
 *       Filename:  socketlib.h
 *
 *    Description:  socket library
 *
 *        Version:  1.0
 *        Created:  03/25/2016 01:00:52 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jignesh Darji (jignesh), jndarji@ncsu.edu
 *   Organization:  North Carolina State University
 *
 * =====================================================================================
 */

#ifndef __SOCKETLIB_H__
#define __SOCKETLIB_H__

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "debug.h"
#include <pthread.h>
#include <arpa/inet.h>

int sendMessageOnSocket(int filedes, char* nullTermString);
int readMessageOnSocket(int filedes);
pthread_t makeSingleClientServer(int port);
void* socketServerListener(void* sock);
void* socketClientListener(void* sock);
pthread_t makeMultiClientServer(/*TODO: call back*/ int port );
void* multiClientServerPacketListener(void* sock);
int createServerSocket(int port);
pthread_t makeClient(char* host, int port);
int createClientSocketAndConnect(char* host, int port);
struct sockaddr_in getPeerFromSocketFD(int sock);
int registerClientConnectedCallback(void *cb);

#endif
