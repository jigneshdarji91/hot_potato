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
#include <sys/time.h>

#define MAX_PLAYERS 100
//FIXME: defined in all files
#define MAX_MSG_LEN 9616

int noOfPlayersInRing;
int noOfPlayersConnected;
int noOfLeftPortsRegistered;
int noOfPlayersComplete;
int masterPort;
int masterSocketFD = 3; //FIXME: get from the socketlib somehow through callback
int noOfHops;
int isPotatoSent = 0;

pthread_t threadId;
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
    threadId = makeMultiClientServer(masterPort);

    char host[64];
    gethostname(host, sizeof(host));
    fprintf(stdout, "Potato Master on %s\n", gethostbyname(host)->h_name);
    fprintf(stdout, "Players = %d\n", noOfPlayersInRing);
    fprintf(stdout, "Hops = %d\n", noOfHops);

    pthread_join(threadId, NULL);
    log_dbg("end");
}

int registerEventHandlers()
{
    registerClientConnectedCallback(playerConnectedEventHandler);
    registerLeftPortReceivedOnMasterCallback(leftPortReceivedHandler);
    registerRightACKReceivedOnMasterCallback(rightACKReceivedHandler);
    registerPotatoReceivedCallback(potatoReceivedHandler);
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
    log_dbg("begin sockfd: %d player: %d", sockfd, getPlayerIDFromSockFD(sockfd));
    log_inf("Server: connect from host %s, port %hd\n",
            inet_ntoa (playerSock->sin_addr),
            ntohs (playerSock->sin_port));

    int playerID = noOfPlayersConnected;
    playerList[playerID].playerID = playerID;
    playerList[playerID].socketFD = sockfd;
    playerList[playerID].northSockInfo = *playerSock;
    playerList[playerID].isRightConnected = 0;

    noOfPlayersConnected++;

    struct hostent *playerHostent;
    playerHostent = gethostbyaddr((char*) &playerList[playerID].northSockInfo.sin_addr,
            sizeof(struct in_addr), AF_INET);
    fprintf(stdout, "player %d is on %s\n", 
            playerList[playerID].playerID, 
            /* gethostbyaddr(inet_ntoa(playerList[playerID].northSockInfo.sin_addr))->h_name */
            playerHostent->h_name);

    sendIDToPlayer(playerID);
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
    sendRightNeighborInfoToAll();
    log_dbg("end");
}

int sendRightNeighborInfoToAll()
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
            {
                playerList[i].isRightConnected = 1;
                noOfPlayersComplete++;
            }
            else if(playerList[i].isRightConnected == 1)
                log_err("ERROR: Right ACK already received");
            break;
        }
    }

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
    if(!isPotatoSent)
    {
        sendPotato(); 
        isPotatoSent = 1;
    }
    log_dbg("end");
}

int sendIDToPlayer(int playerID)
{
    log_dbg("begin playerID: %d", playerID);
    char message[MAX_MSG_LEN];

    int rightNeighbor = (playerID + 1) % noOfPlayersInRing;
    int leftNeighbor = (playerID == 0)?(noOfPlayersInRing - 1):(playerID - 1);
    createPlayerIDMessage(playerID,
            leftNeighbor,
            rightNeighbor,
            message);
    sendMessageOnSocket(playerList[playerID].socketFD, message);

    log_dbg("end playerID: %d leftID: %d rightID: %d", playerID, leftNeighbor, rightNeighbor);
}

int sendPotato()
{
    log_dbg("begin");
    char message[MAX_MSG_LEN];
    struct timeval currTime;
    gettimeofday(&currTime, NULL);
    //srand(currTime.tv_usec);
    srand(time(NULL));
    int r = rand() % noOfPlayersInRing;

    if(noOfHops > 0)
    {
        createPotatoMessage(noOfHops, "", message);
        sendMessageOnSocket(playerList[r].socketFD, message);
        fprintf(stdout, "All players present, sending potato to player %d\n", r);
    }
    else
    {
        potatoReceivedHandler(0, noOfHops, "");
    }

    log_dbg("end");
}

int getPlayerIDFromSockFD(int sockfd)
{
    int i = 0, playerID = -1;
    for(; i < noOfPlayersConnected; ++i)
    {
        if(playerList[i].socketFD == sockfd)
        {
            playerID = playerList[i].playerID;
        }
    }
    log_dbg("sockfd: %d playerID: %d", sockfd, playerID);
    return playerID;
}

int potatoReceivedHandler(int sockfd, int hopsLeft, char* path)
{
    //NOTE: sockfd 0 would imply that the noOfHops were 0 on input and the potato never entered the network
    log_dbg("begin sockfd: %d hopsLeft: %d path: %s", sockfd, hopsLeft, path);
    
    fprintf(stdout, "Trace of potato:\n%s\n", path);
    shutdownAllPlayers();
    shutdownSockets();
    shutdownListenerThreads();
    log_dbg("end");
}

int shutdownAllPlayers()
{
    log_dbg("begin");
    char message[MAX_MSG_LEN];
    int i = 0;
    for(; i < noOfPlayersConnected; ++i)
    {
        createShutdownMessage(message);
        sendMessageOnSocket(playerList[i].socketFD, message);
    }
    log_dbg("end");
}

int shutdownSockets()
{
    log_dbg("begin");

    int i = 0;
    for(; i < noOfPlayersConnected; ++i)
    {
        shutdown(playerList[i].socketFD, 2);
    }

    shutdown(masterPort, 2);

    log_dbg("end");
}

int shutdownListenerThreads()
{
    log_dbg("stopping server thread");
    pthread_cancel(threadId);
}

