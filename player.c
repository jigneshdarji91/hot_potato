/*
 * =====================================================================================
 *
 *       Filename:  player.c
 *
 *    Description:  Player
 *
 *        Version:  1.0
 *        Created:  03/15/2016 09:04:25 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jignesh Darji (jignesh), jndarji@ncsu.edu
 *   Organization:  North Carolina State University
 *
 * =====================================================================================
 */

/* Tasks
 * 1. Connect to Master
 * 2. Open left (and/or right?) sockets
 * 2. Wait for neighbor info
 * 3. On receiving (neighbor infor) packet from Master, connect to (both/left/right?) neighbor(s) 
 * 4. On receiving potato, check TTL;
 *  a. If non-zero, send to right neighbor
 *  b. Else, append signature and send to Master and close program */

#include "player.h"

int masterPort;
int leftPort = 0; //FIXME: random-port server start

char* log_filename = "player.log";

int main (int argc, char *argv[])
{
    if( argc < 3)
    {
        fprintf(stderr, "Usage: %s <master IP> <master port> \n", argv[0]);
        exit(1);
    }

    masterPort = atoi(argv[2]);
    log_inf("player: master=%s port=%d", argv[1], masterPort);
    pthread_t leftThreadId = makeSingleClientServer(leftPort);
    pthread_t masterThreadId = makeClient(argv[1], masterPort);

    pthread_join(masterThreadId, NULL);
    pthread_join(leftThreadId, NULL);
}
