/*
 * =====================================================================================
 *
 *       Filename:  playerinfo.h
 *
 *    Description:  stuct to keep player info
 *
 *        Version:  1.0
 *        Created:  03/25/2016 05:07:51 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jignesh Darji (jignesh), jndarji@ncsu.edu
 *   Organization:  North Carolina State University
 *
 * =====================================================================================
 */

#ifndef __PLAYERINFO_H__
#define __PLAYERINFO_H__

#include "debug.h"

typedef struct {
    int playerID;
    int socketFD;
    int isRightConnected;
    struct sockaddr_in leftSockInfo;
    struct sockaddr_in northSockInfo;
} PlayerInfo;

#endif
