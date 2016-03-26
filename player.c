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

PlayerInfo selfInfo, masterInfo, leftInfo, rightInfo;
int isMasterConnected= 0;
int isLeftServerStarted = 0;
int leftPort = 0; 

pthread_t leftThreadId, rightThreadId, masterThreadId;
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
    leftThreadId = makeSingleClientServer(leftPort);
    masterThreadId = makeClient(argv[1], masterPort);

    pthread_join(leftThreadId, NULL);
    pthread_join(rightThreadId, NULL);
    pthread_join(masterThreadId, NULL);

    log_dbg("end");
}

int registerEventHandlers()
{
    registerClientConnectedCallback(leftNeighborConnectedEventHandler);
    registerServerConnectedCallback(masterConnectedEventHandler);
    registerServerStartedCallback(serverStartedEventHandler);
    registerRightInfoReceivedOnPlayerCallback(rightInfoReceivedHandler);
    registerPotatoReceivedCallback(potatoReceivedHandler);
    registerPlayerIDReceivedOnPlayerCallback(playerIDReceivedHandler);
    registerShutdownReceivedOnPlayerCallback(shutdownReceivedHandler);
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


    isLeftServerStarted = 1;

    leftInfo.socketFD = sockfd;
    leftInfo.selfSockInfo = sin;

    log_dbg("end");
}

int leftNeighborConnectedEventHandler(int sockfd, struct sockaddr_in* leftSock)
{

    log_dbg("begin sockfd: %d leftID: %d", sockfd, leftInfo.playerID);
    log_inf("Server: connect from host %s, port %hd\n",
            inet_ntoa (leftSock->sin_addr),
            ntohs (leftSock->sin_port));

    leftInfo.socketFD = sockfd;
    leftInfo.selfSockInfo = *leftSock;
    
    log_dbg("end");
}

int masterConnectedEventHandler(int sockfd, struct sockaddr* sock)
{
    log_dbg("begin sockfd: %d", sockfd);
    if(!isMasterConnected)
    {
        log_inf("master connected");
        masterInfo.socketFD = sockfd;
        masterInfo.northSockInfo = *((struct sockaddr_in *) sock);
        sendLeftPortToMaster(sockfd);
        isMasterConnected = 1;
    }
    else
    {
        log_inf("right player connected");
        rightInfo.socketFD = sockfd;
        rightInfo.leftSockInfo = *((struct sockaddr_in *) sock);
        sendRightACKToMaster(masterInfo.socketFD);
    }
}

int sendLeftPortToMaster(int sockfd)
{
    log_dbg("begin leftPort: %d", ntohs(leftInfo.selfSockInfo.sin_port));

    char message[MAX_MSG_LEN];
    while(!isLeftServerStarted);
    createLeftSocketPortMessage(ntohs(leftInfo.selfSockInfo.sin_port), message);
    sendMessageOnSocket(sockfd, message);

    log_dbg("end");
}

int sendRightACKToMaster(int sockfd)
{
    log_dbg("begin sockfd: %d", sockfd);

    char message[MAX_MSG_LEN];
    createRighACKReceivedMessage(message);
    sendMessageOnSocket(sockfd, message);

    log_dbg("end");
}

int rightInfoReceivedHandler(int sockfd, char* host, int port)
{
    log_dbg("begin sockfd: %d host: %s port: %d", sockfd, host, port);
    rightThreadId = makeClient(host, port);
    sendRightACKToMaster(sockfd);
    log_dbg("end");
}

int potatoReceivedHandler(int sockfd, int hopsLeft, char* pathReceived)
{
    log_dbg("begin playerID: %d hopsLeft: %d", selfInfo.playerID, hopsLeft);

    char message[MAX_MSG_LEN];
    char path[MAX_MSG_LEN];
    if(pathReceived == NULL)
        strcpy(path, "");
    else
    {
        strcpy(path, pathReceived);
        strcat(path, ","); 
    }
    char playerIDString[32];
    sprintf(playerIDString, "%d", selfInfo.playerID);
    strcat(path, playerIDString);
    log_inf("potato path: %s", path);

    //TODO: verify whether to return on 1 or 0
    if(hopsLeft)
    {
        hopsLeft--;
        createPotatoMessage(hopsLeft, path, message);
        int r = rand() % 2;
        //FIXME: This is for test purposes only
        /*if(r == 1)
        {
            fprintf(stdout, "Sending potato to %d", leftInfo.playerID);
            log_inf("Sending potato to %d sockfd: %d", leftInfo.playerID, leftInfo.socketFD);
            sendMessageOnSocket(leftInfo.socketFD, message);
        }
        else */
        {
            fprintf(stdout, "Sending potato to %d", rightInfo.playerID);
            log_inf("Sending potato to %d sockfd: %d", rightInfo.playerID, rightInfo.socketFD);
            sendMessageOnSocket(rightInfo.socketFD, message); 
        }
    }
    else
    {
        fprintf(stdout, "I'm it\n");
        createPotatoMessage(hopsLeft, path, message);
        sendMessageOnSocket(masterInfo.socketFD, message); 
        shutdownSockets();
    }

    log_dbg("end");
}

int playerIDReceivedHandler(int sockfd, int selfID, int leftID, int rightID)
{
    log_dbg("begin selfID: %d leftID: %d rightID: %d", selfID, leftID, rightID);

    fprintf(stdout, "Connected as player %d", selfID);

    selfInfo.playerID = selfID;
    leftInfo.playerID = leftID;
    rightInfo.playerID = rightID;

    log_dbg("end");
}

int shutdownReceivedHandler(int sockfd)
{
    log_dbg("shutting down");
    shutdownSockets();
}

int shutdownSockets()
{
    log_dbg("begin");
    
    shutdown(leftInfo.socketFD, 2);
    shutdown(masterInfo.socketFD, 2);
    shutdown(rightInfo.socketFD, 2);

    log_dbg("end");
}
