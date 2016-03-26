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
typedef int (* playerIDReceivedCallback)(int sockfd, int port);
typedef int (* rightACKReceivedCallback)(int sockfd);
typedef int (* rightInfoReceivedCallback)(int sockfd, char* host, int portno);

leftPortReceivedCallback leftPortReceived = NULL;
playerIDReceivedCallback playerIDReceived = NULL;
rightACKReceivedCallback rightACKReceived = NULL;
rightInfoReceivedCallback rightInfoReceived = NULL; 

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
    log_inf("begin sockfd: %d received message: %s", sockfd, message);
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
        if(!strstr(messageType, "LEFTPORT"))
        {
            parseMessageLeftPortOnMaster(sockfd, message);
        }
        if(!strstr(messageType, "RIGHTCONNECTED"))
        {
            parseMessageRightConnectedOnMaster(sockfd, message);
        }
        if(!strstr(messageType, "PLAYERID"))
        {
            parseMessagePlayerIDOnPlayer(sockfd, message);
        }
        if(!strstr(messageType, "RIGHTINFO"))
        {
            parseMessageRightInfoOnPlayer(sockfd, message);
        }
        if(!strstr(messageType, "POTATO"))
        {
            parsePotatoOnPlayer(sockfd, message);
        }
    }

    log_inf("end");
}

int parseMessageLeftPortOnMaster(int sockfd, char* message)
{
    log_inf("begin sockfd: %d received message: %s", sockfd, message);
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

}

int parseMessageRightInfoOnPlayer(int sockfd, char* message)
{
    log_inf("begin sockfd: %d received message: %s", sockfd, message);
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
        log_dbg("testing messageSplitForHost: %s", messageSplitForHost);
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
        log_dbg("testing messageSplitForPort: %s", messageSplitForPort);
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

int parsePotatoOnPlayer(int sockfd, char* message)
{

}
int parsePotatoOnMaster(int sockfd, char* message)
{

}

