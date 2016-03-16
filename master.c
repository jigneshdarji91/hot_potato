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

static int noOfPlayersInRing;
static int noOfPlayersConnected;


// Callback function from listener if connection is established
void registerPlayerEvent() 
{
    //add info to connectedPlayerMap     
    noOfPlayersConnected++;
    checkAndSetupRing();
}

void checkAndSetupRing()
{
    if(isEveryoneConnectedToMaster())
        everyoneConnectedEvent();
}

void playerConnectedToNeighborEvent(p)
{
    //update PlayerInfo
    if(isRingComplete())
        sendPotato();
}

void everyoneConnectedEvent()
{
    sendNeighborInfoToEveryone();
}

void sendNeighborInfoToEveryone()
{
    //for all players p
    sendNeighborInfoToPlayer(p);
}

void sendNeighborInfoToPlayer(PlayerInfo p)
{
    //encode p->right in message such that serialization is possible
}

bool isEveryoneConnectedToMaster()
{
    return (noOfPlayersInRing == noOfPlayersConnected)? true : false;
}

bool isRingComplete()
{
    //for each PlayerInfo, check neighborConnected flag
}

void sendPotato()
{
    // 1. Pick a random player
    // 2. Send
    // 3. Wait
}

void killPlayer(PlayerInfo p)
{

}

void killNetwork()
{
    //for all PlayerInfo p
    killPlayer(p);
}

void potatoReturnEvent()
{
    printPotatoPath();
    killNetwork();
}

void printPotatoPath()
{

}

