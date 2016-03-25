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
#include <string.h>

//FIXME: defined in master.c
#define MAX_MSG_LEN 9616

int createLeftSocketPortMessage(int port, char* message)
{
    log_dbg("begin");
    
    strcpy(message, "MESSAGE_TYPE:LEFTPORT;");
    strcat(message, "PORT:");
    char portString[32];
    sprintf(portString, "%d", port);
    strcat(message, portString);

    log_dbg("end messsage: %s", message);
}

int parseMessage(char* message)
{
    log_inf("begin received message: %s", message);
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

    log_inf("end");
}

