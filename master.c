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

#define MAX_PLAYERS 100
//FIXME: defined in all files
#define MAX_MSG_LEN 9616

int noOfPlayersInRing;
int noOfPlayersConnected;
int noOfLeftPortsRegistered;
int noOfPlayersComplete;
int masterPort;
int noOfHops;

PlayerInfo playerList[MAX_PLAYERS];
// char* log_filename = "master.log";

int main (int argc, char *argv[])
{

    if( argc < 4)
    {
        fprintf(stderr, "Usage: %s <port> <no. of players> <hops>\n", argv[0]);
        exit(1);
    }

    noOfPlayersInRing = atoi(argv[2]);
    noOfPlayersConnected = 0;
    noOfLeftPortsRegistered = 0;
    noOfPlayersComplete = 0;

    masterPort = atoi(argv[1]);
    noOfHops = atoi(argv[3]);

    registerEventHandlers();

    log_inf("master: port=%d players=%d hops=%d", masterPort, noOfPlayersInRing, noOfHops);
    pthread_t threadId = makeMultiClientServer(masterPort);
    pthread_join(threadId, NULL);
}

int registerEventHandlers()
{
    registerClientConnectedCallback(playerConnectedEventHandler);
    registerLeftPortReceivedOnMasterCallback(leftPortReceivedHandler);
    registerRightACKReceivedOnMasterCallback(rightACKReceivedHandler);
}

int leftPortReceivedHandler(int sockfd, int port)
{
    log_dbg("begin sockfd: %d port: %d htons(port): %d", sockfd, port, htons(port));
    int i = 0;
    for(; i < noOfPlayersConnected; ++i)
    {
        if(playerList[i].socketFD == sockfd)
        {
            playerList[i].leftSockInfo.sin_family = AF_INET;
            playerList[i].leftSockInfo.sin_port = htons(port);
            playerList[i].leftSockInfo.sin_addr.s_addr = playerList[i].northSockInfo.sin_addr.s_addr;
            break;
        }
    }
    noOfLeftPortsRegistered++;
    if(noOfLeftPortsRegistered == noOfPlayersInRing)
    {
        allPlayersConnectedEvent();
    }
    log_dbg("end");
}

int playerConnectedEventHandler(int sockfd, struct sockaddr_in* playerSock)
{
    log_dbg("begin sockfd: %d", sockfd);
    log_inf("Server: connect from host %s, port %hd\n",
            inet_ntoa (playerSock->sin_addr),
            ntohs (playerSock->sin_port));

    playerList[noOfPlayersConnected].playerID = noOfPlayersConnected;
    playerList[noOfPlayersConnected].socketFD = sockfd;
    playerList[noOfPlayersConnected].northSockInfo = *playerSock;
    playerList[noOfPlayersConnected].isRightConnected = 0;

    noOfPlayersConnected++;
    log_dbg("end noOfPlayersConnected: %d", noOfPlayersConnected);
}

int allPlayersConnectedEvent()
{
    log_dbg("begin");
    int i = 0;
    for(; i < noOfPlayersConnected; ++i)
    {
        log_inf("ID: %d host: %s port: %d", 
            playerList[i].playerID, 
            inet_ntoa(playerList[i].leftSockInfo.sin_addr),
            ntohs(playerList[i].leftSockInfo.sin_port));
    }
    sendRightNeighborInfoToAllPlayers();
    log_dbg("end");
}

int sendRightNeighborInfoToAllPlayers()
{
    log_dbg("begin");

    char message[MAX_MSG_LEN];

    int i = 0;
    for(;i < noOfPlayersInRing; i++)
    {
        char* host = inet_ntoa(playerList[(i + 1)%noOfPlayersInRing].leftSockInfo.sin_addr);
        char port[32];
        sprintf(port, "%d", ntohs(playerList[(i + 1)%noOfPlayersInRing].leftSockInfo.sin_port));
        createRightNeighborInfoMessage(host, port, message);
        sendMessageOnSocket(playerList[i].socketFD, message);
    }

    log_dbg("end");
}

int rightACKReceivedHandler(int sockfd)
{
    log_dbg("begin sockfd: %d ", sockfd);
    int i = 0;
    for(; i < noOfPlayersConnected; ++i)
    {
        if(playerList[i].socketFD == sockfd)
        {
            if(playerList[i].isRightConnected == 0)
                playerList[i].isRightConnected = 1;
            else
                log_err("ERROR: Right ACK already received");
        }
    }

    noOfPlayersComplete++;
    if(noOfPlayersComplete == noOfPlayersInRing)
    {
        ringCompleteEvent();
    }
    log_dbg("end");
}

int ringCompleteEvent()
{
    log_dbg("begin");
    //LET THE GAME OF POTATO BEGIN 
    
    char message[MAX_MSG_LEN];
    int r = rand() % noOfPlayersInRing;

    createPotatoMessage(noOfHops, "", message);
    sendMessageOnSocket(playerList[r].socketFD, message);

    log_dbg("end");
}

