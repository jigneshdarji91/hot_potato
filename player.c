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
#include "potato_protocol.h"

#define MAX_MSG_LEN 9616

int masterPort;

PlayerInfo selfInfo, masterInfo;
int isLeftServerStarted = 0;
int leftSocketFD; 
int leftPort = 0; 
struct sockaddr_in leftSocketAddr;

//char* log_filename = "player.log";

int main (int argc, char *argv[])
{
    if( argc < 3)
    {
        fprintf(stderr, "Usage: %s <master IP> <master port> \n", argv[0]);
        exit(1);
    }

    registerEventHandlers();

    masterPort = atoi(argv[2]);
    log_inf("player: master=%s port=%d", argv[1], masterPort);
    pthread_t leftThreadId = makeSingleClientServer(leftPort);
    pthread_t masterThreadId = makeClient(argv[1], masterPort);

    pthread_join(masterThreadId, NULL);
    pthread_join(leftThreadId, NULL);
}

int registerEventHandlers()
{
    registerMasterConnectedEventHandler();
    registerServerStartedEventHandler();
}
int registerMasterConnectedEventHandler()
{
    registerServerConnectedCallback(masterConnectedEventHandler);
}

int registerServerStartedEventHandler()
{
    registerServerStartedCallback(serverStartedEventHandler);
}

int serverStartedEventHandler(int sockfd, struct sockaddr_in* leftSock)
{

    //FIXME: the leftSock comes blank as we're using random port.
    // Currently using getsockname as a workaround. 
    log_dbg("begin sockfd: %d", sockfd);

    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(sockfd, (struct sockaddr *)&sin, &len) == -1)
            perror("getsockname");

    log_inf("host %s, port %hd\n",
            inet_ntoa (sin.sin_addr),
            ntohs (sin.sin_port));

    leftSocketFD = sockfd;
    leftSocketAddr = sin;

    isLeftServerStarted = 1;

    log_dbg("end");
}

int masterConnectedEventHandler(int sockfd, struct sockaddr* masterSock)
{
    log_dbg("begin sockfd: %d", sockfd);

    masterInfo.socketFD = sockfd;
    masterInfo.northSockInfo = *((struct sockaddr_in *) masterSock);

    sendMasterLeftPortInfo(sockfd);
}

int sendMasterLeftPortInfo(int sockfd)
{
    log_dbg("begin leftPort: %d", ntohs(leftSocketAddr.sin_port));

    char message[MAX_MSG_LEN];
    while(!isLeftServerStarted);
    createLeftSocketPortMessage(ntohs(leftSocketAddr.sin_port), message);
    sendMessageOnSocket(sockfd, message);

    log_dbg("end message: %s", message);
}
