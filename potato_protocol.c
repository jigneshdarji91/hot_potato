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
