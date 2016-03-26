/*
 * =====================================================================================
 *
 *       Filename:  potato_protocol.c
 *
 *    Description:  Potato Protocol 
 *
 *        Version:  1.0
 *        Created:  03/25/2016 05:47:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jignesh Darji (jignesh), jndarji@ncsu.edu
 *   Organization:  North Carolina State University
 *
 * =====================================================================================
 */

#include "potato_protocol.h"
#include <stdlib.h>
#include <string.h>

//FIXME: defined in master.c
#define MAX_MSG_LEN 9616

typedef int (* leftPortReceivedCallback)(int sockfd, int port);
typedef int (* playerIDReceivedCallback)(int sockfd, int selfID, int leftID, int rightID);
typedef int (* rightACKReceivedCallback)(int sockfd);
typedef int (* rightInfoReceivedCallback)(int sockfd, char* host, int portno);
typedef int (* potatoReceivedCallback)(int sockfd, int hopsLeft, char* path);

leftPortReceivedCallback leftPortReceived = NULL;
playerIDReceivedCallback playerIDReceived = NULL;
rightACKReceivedCallback rightACKReceived = NULL;
rightInfoReceivedCallback rightInfoReceived = NULL; 
potatoReceivedCallback potatoReceived = NULL;

int registerPotatoReceivedCallback(void *cb)
{
    potatoReceived = (potatoReceivedCallback) cb;
}

int registerLeftPortReceivedOnMasterCallback(void *cb)
{
    leftPortReceived = (leftPortReceivedCallback) cb;
}

int registerRightACKReceivedOnMasterCallback(void *cb)
{
    rightACKReceived = (rightACKReceivedCallback) cb;
}

int registerPlayerIDReceivedOnPlayerCallback(void *cb)
{
    playerIDReceived = (playerIDReceivedCallback) cb;
}

int registerRightInfoReceivedOnPlayerCallback(void *cb)
{
    rightInfoReceived = (rightInfoReceivedCallback) cb;
}

int createPotatoMessage(int noOfHops, char* path, char* message)
{
    log_dbg("begin");
    
    strcpy(message, "MESSAGE_TYPE:POTATO;");
    
    strcat(message, "PATH:");
    strcat(message, path);
    strcat(message, ";");

    char hopString[32];
    sprintf(hopString, "%d", noOfHops);
    strcat(message, "HOPCOUNT:");
    strcat(message, hopString);
    strcat(message, ";");

    log_dbg("end messsage: %s", message);
}

int createPlayerIDMessage(int playerID, int leftNeighborID, int rightNeighborID, char* message)
{
    log_dbg("begin");

    strcpy(message, "MESSAGE_TYPE:PLAYERID;");

    char pID[32], leftID[32], rightID[32];
    sprintf(pID, "%d", playerID);
    sprintf(leftID, "%d", leftNeighborID);
    sprintf(rightID, "%d", rightNeighborID);

    strcat(message, "YOURID:");
    strcat(message, pID);
    strcat(message, ";");
    strcat(message, "LEFTID:");
    strcat(message, leftID);
    strcat(message, ";");
    strcat(message, "RIGHTID:");
    strcat(message, rightID);
    strcat(message, ";");

    log_dbg("end");
}

int createLeftSocketPortMessage(int port, char* message)
{
    log_dbg("begin");
    
    strcpy(message, "MESSAGE_TYPE:LEFTPORT;");
    //NOTE: making PORT as the field comes in the way of the parser
    strcat(message, "PORTNO:");
    char portString[32];
    sprintf(portString, "%d", port);
    strcat(message, portString);
    strcat(message, ";");

    log_dbg("end messsage: %s", message);
}

int createRightNeighborInfoMessage(char* host, char* port, char* message)
{
    log_dbg("begin");
    
    strcpy(message, "MESSAGE_TYPE:RIGHTINFO;");
    //NOTE: making PORT as the field comes in the way of the parser
    strcat(message, "HOST:");
    strcat(message, host);
    strcat(message, ";");
    strcat(message, "PORTNO:");
    strcat(message, port);
    strcat(message, ";");

    log_dbg("end messsage: %s", message);
}

int createRighACKReceivedMessage(char* message)
{
    strcpy(message, "MESSAGE_TYPE:RIGHTCONNECTED;\0");

    log_dbg("messsage: %s", message);
}

int parseMessage(int sockfd, char* message)
{
    log_dbg("begin sockfd: %d received message: %s", sockfd, message);
    char messageToParse[MAX_MSG_LEN];
    char* messageType = "";
    strcpy(messageToParse, message);

    char *messageSplit = strtok(messageToParse, ";");
    while(messageSplit != NULL)
    {
        if(NULL != strstr(messageSplit, "MESSAGE_TYPE"))
        {
            messageType = strtok(messageSplit, ":");
            messageType = strtok(NULL, ":");
            log_inf("section: %s message type: %s", messageSplit, messageType);
            break;
        }
        messageSplit = strtok(NULL, ";");
    } 

    if(messageType != NULL)
    {
        if(!strcmp(messageType, "LEFTPORT"))
        {
            parseMessageLeftPortOnMaster(sockfd, message);
        }
        if(!strcmp(messageType, "RIGHTCONNECTED"))
        {
            parseMessageRightConnectedOnMaster(sockfd, message);
        }
        if(!strcmp(messageType, "PLAYERID"))
        {
            parseMessagePlayerIDOnPlayer(sockfd, message);
        }
        if(!strcmp(messageType, "RIGHTINFO"))
        {
            parseMessageRightInfoOnPlayer(sockfd, message);
        }
        if(!strcmp(messageType, "POTATO"))
        {
            parsePotato(sockfd, message);
        }
    }

    log_dbg("end");
}

int parsePotato(int sockfd, char* message)
{
    log_dbg("begin sockfd: %d received message: %s", sockfd, message);
    char messageToParseForHops[MAX_MSG_LEN];
    char messageToParseForPath[MAX_MSG_LEN];
    char* hopsLeft = "";
    char* path = "";
    strcpy(messageToParseForHops, message);
    strcpy(messageToParseForPath, message);

    char *messageSplitForHops = strtok(messageToParseForHops, ";");

    while(messageSplitForHops != NULL)
    {
        if(NULL != strstr(messageSplitForHops, "HOPCOUNT:"))
        {
            hopsLeft = strtok(messageSplitForHops, ":");
            hopsLeft = strtok(NULL, ":");
            log_inf("section: %s hops left: %s", messageSplitForHops, hopsLeft);
        }
        messageSplitForHops = strtok(NULL, ";");
    } 

    char *messageSplitForPath = strtok(messageToParseForPath, ";");
    while(messageSplitForPath != NULL)
    {
        if(NULL != strstr(messageSplitForPath, "PATH:"))
        {
            path = strtok(messageSplitForPath, ":");
            path = strtok(NULL, ":");
            log_inf("section: %s path: %s", messageSplitForPath, path);
        }
        messageSplitForPath = strtok(NULL, ";");
    } 

    if(hopsLeft != NULL)
    {
        if(potatoReceived != NULL)
            potatoReceived(sockfd, atoi(hopsLeft), path);
    }

    log_dbg("end");
}

int parseMessageLeftPortOnMaster(int sockfd, char* message)
{
    log_dbg("begin sockfd: %d received message: %s", sockfd, message);
    char messageToParse[MAX_MSG_LEN];
    char* messagePort = "";
    strcpy(messageToParse, message);

    char *messageSplit = strtok(messageToParse, ";");
    while(messageSplit != NULL)
    {
        if(NULL != strstr(messageSplit, "PORTNO"))
        {
            messagePort = strtok(messageSplit, ":");
            messagePort= strtok(NULL, ":");
            log_inf("section: %s port: %s", messageSplit, messagePort);
            break;
        }
        messageSplit = strtok(NULL, ";");
    } 

    if(messagePort != NULL)
    {
        if(leftPortReceived != NULL)
            leftPortReceived(sockfd, atoi(messagePort));
    }

    log_dbg("end");
}

int parseMessageRightConnectedOnMaster(int sockfd, char* message)
{
    log_dbg("");
    rightACKReceived(sockfd);
}

int parseMessagePlayerIDOnPlayer(int sockfd, char* message)
{
    log_dbg("begin sockfd: %d received message: %s", sockfd, message);

    char messageToParseForSelfID[MAX_MSG_LEN];
    char messageToParseForLeftID[MAX_MSG_LEN];
    char messageToParseForRightID[MAX_MSG_LEN];

    char* selfID = "";
    char* leftID= "";
    char* rightID = "";

    strcpy(messageToParseForSelfID, message);
    strcpy(messageToParseForLeftID, message);
    strcpy(messageToParseForRightID, message);

    char *messageSplitForSelfID = strtok(messageToParseForSelfID, ";");
    while(messageSplitForSelfID != NULL)
    {
        if(NULL != strstr(messageSplitForSelfID, "YOURID:"))
        {
            selfID = strtok(messageSplitForSelfID, ":");
            selfID = strtok(NULL, ":");
            log_inf("selfID: %s", selfID);
        }
        messageSplitForSelfID = strtok(NULL, ";");
    } 

    char *messageSplitForLeftID = strtok(messageToParseForLeftID, ";");
    while(messageSplitForLeftID != NULL)
    {
        if(NULL != strstr(messageSplitForLeftID, "LEFTID:"))
        {
            leftID = strtok(messageSplitForLeftID, ":");
            leftID = strtok(NULL, ":");
            log_inf("leftID: %s", leftID);
        }
        messageSplitForLeftID = strtok(NULL, ";");
    }

    char *messageSplitForRightID = strtok(messageToParseForRightID, ";");
    while(messageSplitForRightID != NULL)
    {
        if(NULL != strstr(messageSplitForRightID, "RIGHTID:"))
        {
            rightID = strtok(messageSplitForRightID, ":");
            rightID = strtok(NULL, ":");
            log_inf("rightID: %s", rightID);
        }
        messageSplitForRightID = strtok(NULL, ";");
    } 


    if(selfID != NULL && leftID != NULL && rightID != NULL)
    {
        if(playerIDReceived != NULL)
            playerIDReceived(sockfd, atoi(selfID), atoi(leftID), atoi(rightID));
    }

    log_dbg("end");

}

int parseMessageRightInfoOnPlayer(int sockfd, char* message)
{
    log_dbg("begin sockfd: %d received message: %s", sockfd, message);
    char messageToParseForHost[MAX_MSG_LEN];
    char messageToParseForPort[MAX_MSG_LEN];
    char* host = "";
    char* portno = "";
    strcpy(messageToParseForHost, message);
    strcpy(messageToParseForPort, message);

    log_dbg("messageToParseForHost: %s", messageToParseForHost);
    log_dbg("messageToParseForPort: %s", messageToParseForPort);
    char *messageSplitForHost = strtok(messageToParseForHost, ";");

    while(messageSplitForHost != NULL)
    {
        if(NULL != strstr(messageSplitForHost, "HOST:"))
        {
            host = strtok(messageSplitForHost, ":");
            host = strtok(NULL, ":");
            log_inf("section: %s host: %s", messageSplitForHost, host);
        }
        messageSplitForHost = strtok(NULL, ";");
    } 

    char *messageSplitForPort = strtok(messageToParseForPort, ";");
    while(messageSplitForPort != NULL)
    {
        if(NULL != strstr(messageSplitForPort, "PORTNO"))
        {
            portno = strtok(messageSplitForPort, ":");
            portno = strtok(NULL, ":");
            log_inf("section: %s portno: %s", messageSplitForPort, portno);
        }
        messageSplitForPort = strtok(NULL, ";");
    } 

    if(host != NULL && portno != NULL)
    {
        if(rightInfoReceived!= NULL)
            rightInfoReceived(sockfd, host, atoi(portno));
    }

    log_dbg("end");
}

