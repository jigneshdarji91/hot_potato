/*
 * =====================================================================================
 *
 *       Filename:  master.h
 *
 *    Description:  Ring Master
 *
 *        Version:  1.0
 *        Created:  03/15/2016 09:16:23 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jignesh Darji (jignesh), jndarji@ncsu.edu
 *   Organization:  North Carolina State University
 *
 * =====================================================================================
 */

typedef unsigned int PlayerID;
typedef struct PlayerInfo 
{
    PlayerID id;
    //socketInfo;
    //leftNeighbor?
    //rightNeighbor?
    
    //leftConnected (either or) rightConnected
} PlayerInfo;

map<PlayerID, PlayerInfo> connectedPlayerMap;
