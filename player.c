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


void connectToMaster()
{

}

void listenForLeftNeighbor()
{

}

void leftNeighborConnected()
{

}

void neighborInfoSentByMaster()
{
    connectToRightNeighbor();
}

void checkConnectionAndInformMaster()
{
    if(isConnectedToBothNeighbors())
        informMasterConnectionComplete();
}

void informMasterConnectionComplete()
{

}

void connectToRightNeighbor()
{

}

bool isConnectedToBothNeighbors()
{
    if(selfInfo->isLeftConnected &&
        selfInfo->isRightConnected)
        return true;
    return false;
}

void potatoReceivedEvent()
{
    appendSignatureToPotato();
    sendPotatoToRandomNeighbor();
}

void appendSignatureToPotato()
{

}

void sendPotatoToRandomNeighbor()
{

}

void killSelfEvent()
{
    closeConnections();
    //exit(0);
}

void closeConnections()
{
    closeLeftConnection();
    closeRightConnection();
}

void closeLeftConnection()
{

}

void closeRightConnection()
{

}


