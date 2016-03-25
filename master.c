/*
 * =====================================================================================
 *
 *       Filename:  master.c
 *
 *    Description:  Ring Master
 *
 *        Version:  1.0
 *        Created:  03/15/2016 08:57:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jignesh Darji (jignesh), jndarji@ncsu.edu
 *   Organization:  North Carolina State University
 *
 * =====================================================================================
 */

/* Tasks
 * 1. Create n listeners
 * 2. When connected, inform player about it's neighbor
 * 3. Wait for all players to finish step 2.
 * 4. Send the packet with hop count configured (in the message? or IP field?)
 * 5. Wait for packet to return
 * 6. Print the stack and kill them all
 */

#include "master.h"

static int noOfPlayersInRing;
static int noOfPlayersConnected;
int masterPort;
int noOfHops;

char* log_filename = "master.log";

int main (int argc, char *argv[])
{

    if( argc < 4)
    {
        fprintf(stderr, "Usage: %s <port> <no. of players> <hops>\n", argv[0]);
        exit(1);
    }

    masterPort = atoi(argv[1]);
    noOfPlayersInRing = atoi(argv[2]);
    noOfHops = atoi(argv[3]);
    log_inf("master: port=%d players=%d hops=%d", masterPort, noOfPlayersInRing, noOfHops);
    pthread_t threadId = makeMultiClientServer(masterPort);
    pthread_join(threadId, NULL);
}
