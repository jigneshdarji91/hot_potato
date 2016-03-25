/*
 * =====================================================================================
 *
 *       Filename:  socketlib.c
 *
 *    Description:  custom socket library 
 *
 *        Version:  1.0
 *        Created:  03/24/2016 10:07:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jignesh Darji (jignesh), jndarji@ncsu.edu
 *   Organization:  North Carolina State University
 *
 * =====================================================================================
 */

#include "socketlib.h"

#define MAXMSG  512

typedef int (* clientConnectedCallback)(int sockfd, struct sockaddr_in* clientSock);
typedef int (* serverConnectedCallback)(int sockfd, struct sockaddr* serverSock);
typedef int (* serverStartedCallback)(int sockfd, struct sockaddr_in* serverSock);

clientConnectedCallback clientConnected = NULL; 
serverConnectedCallback serverConnected = NULL; 
serverStartedCallback serverStarted = NULL; 

int registerClientConnectedCallback(void *cb)
{
    clientConnected = (clientConnectedCallback)cb;
}

int registerServerConnectedCallback(void *cb)
{
    serverConnected = (serverConnectedCallback) cb;
}

int registerServerStartedCallback(void *cb)
{
    serverStarted = (serverStartedCallback) cb;
}

int sendMessageOnSocket(int filedes, char* nullTermString)
{
    log_dbg("begin socket: %d message: %s strlen: %zd", filedes, nullTermString, strlen(nullTermString));
    int len = write(filedes, nullTermString, strlen(nullTermString));
    if ( len != strlen(nullTermString) ) {
        perror("send");
        exit(1);
    }
    log_dbg("end");
}
int readMessageOnSocket(int filedes)
{
    char buffer[MAXMSG];
    int nbytes;

    nbytes = read (filedes, buffer, MAXMSG);
    if (nbytes < 0)
    {
        /*  Read error. */
        perror ("read");
        exit (EXIT_FAILURE);
    }
    else if (nbytes == 0)
        /*  End-of-file. */
        return -1;
    else
    {
        /*  Data read. */
        fprintf (stderr, "Server: got message: `%s'\n", buffer);
        log_inf("message: `%s'\n", buffer);
        return 0;
    }

    //TODO: this happens on shutdown
    /* close (i);
       FD_CLR (i, &active_fd_set); */
}

pthread_t makeSingleClientServer(int port)
{
    log_dbg("begin");

    int *sock = malloc(sizeof(int));
    *sock = createServerSocket(port);

    if (listen (*sock, 1) < 0)
    {
        perror ("listen");
        exit (EXIT_FAILURE);
    }


    pthread_t listenerThread; 
    pthread_create(&listenerThread, NULL, socketServerListener, (void*) sock );
    
    log_dbg("end");
    return listenerThread;
}

void* socketClientListener(void* sock_)
{
    int sock = *((int*) sock_);
    log_dbg("begin socket: %d FD_SETSIZE: %d", sock, FD_SETSIZE);
    socklen_t size;
    struct sockaddr_in clientname;
    fd_set active_fd_set, read_fd_set;

    /*  Initialize the set of active sockets. */
    FD_ZERO (&active_fd_set);
    FD_SET (sock, &active_fd_set);

    while (1)
    {
        /*  Block until input arrives on one or more
         *  active sockets. */
        read_fd_set = active_fd_set;
        if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0)
        {
            perror ("select");
            exit (EXIT_FAILURE);
        }

        /*  Service all the sockets with input pending. */
        int i = 0;
        for (; i < FD_SETSIZE; ++i)
        {
            if (FD_ISSET (i, &read_fd_set))
            {
                // connected socket 
                readMessageOnSocket(i);
            }
        }
    }
    log_dbg("end");

}

void* socketServerListener(void* sock_)
{
    log_dbg("begin");

    //FIXME: how different should player's listener be different from masters?
    int sock = *((int *) sock_);
    multiClientServerPacketListener(sock_);
    /* 
    struct sockaddr_in clientname;
    socklen_t size;
    fd_set read_fd_set, active_fd_set;

    // Initialize the set of active sockets. 
    FD_ZERO (&active_fd_set);
    FD_SET (sock, &active_fd_set);

    while ( 1 ) {
        if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0)
        {
            perror ("select");
            exit (EXIT_FAILURE);
        }
        
        int i = 0;
        for (; i < FD_SETSIZE; ++i)
        {
            if (FD_ISSET (i, &read_fd_set))
            {
                if(i == sock)
                {
                    // Connection request 
                    int newSock;
                    size = sizeof (clientname);
                    newSock = accept (sock,
                            (struct sockaddr *) &clientname,
                            &size);
                    if (newSock < 0)
                    {
                        perror ("accept");
                        exit (EXIT_FAILURE);
                    }
                    FD_SET (newSock, &active_fd_set);
                    //TODO: client connected
                    log_inf("Server: connect from host %s, port %hd\n",
                            inet_ntoa (clientname.sin_addr),
                            ntohs (clientname.sin_port));
                    log_inf("player connected");
                }
                else
                {
                    if( readMessageOnSocket (i) < 0)
                        break;
                }
            }
        }
    }
    */
    log_dbg("end");
}

pthread_t makeMultiClientServer(/*TODO: call back*/ int port )
{
    int *sock = malloc(sizeof(int *));
    *sock = createServerSocket(port);
    log_dbg("begin port: %d sock: %d", port, *sock);
    if (listen (*sock, 1) < 0)
    {
        perror ("listen");
        exit (EXIT_FAILURE);
    }
    pthread_t listenerThread; 
    pthread_create(&listenerThread, NULL, multiClientServerPacketListener, (void*) sock );
    log_dbg("end");
    return listenerThread;
}

void* multiClientServerPacketListener(void *sock_)
{
    int sock = *((int*) sock_);
    log_dbg("begin socket: %d FD_SETSIZE: %d", sock, FD_SETSIZE);
    socklen_t size;
    struct sockaddr_in clientname;
    fd_set active_fd_set, read_fd_set;

    /*  Initialize the set of active sockets. */
    FD_ZERO (&active_fd_set);
    FD_SET (sock, &active_fd_set);

    while (1)
    {
        /*  Block until input arrives on one or more
         *  active sockets. */
        read_fd_set = active_fd_set;
        if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0)
        {
            perror ("select");
            exit (EXIT_FAILURE);
        }

        /*  Service all the sockets with input pending. */
        int i = 0;
        for (; i < FD_SETSIZE; ++i)
        {
            if (FD_ISSET (i, &read_fd_set))
            {
                if (i == sock)
                {
                    // Connection request 
                    int newSock;
                    size = sizeof (clientname);
                    newSock = accept (sock,
                            (struct sockaddr *) &clientname,
                            &size);
                    if (newSock < 0)
                    {
                        perror ("accept");
                        exit (EXIT_FAILURE);
                    }
                    FD_SET (newSock, &active_fd_set);
                    //TODO: client connected
                    log_inf("Server: connect from host %s, port %hd\n",
                            inet_ntoa (clientname.sin_addr),
                            ntohs (clientname.sin_port));
                    log_inf("player connected");
                    // sendMessageOnSocket(sock, message);
                    if(clientConnected != NULL)
                        clientConnected(newSock, &clientname);
                }
                else
                {
                    // connected socket 
                    readMessageOnSocket(i);
                }
            }
        }
    }
    log_dbg("end");
}

int createServerSocket(int port)
{
    log_dbg("begin port: %d", port);
    int sock;
    struct sockaddr_in sockinfo;

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror ("socket");
        exit (EXIT_FAILURE);
    }

    sockinfo.sin_family = AF_INET;
    sockinfo.sin_port = htons (port);
    //TODO: can use gethostbyname call here
    sockinfo.sin_addr.s_addr = htonl (INADDR_ANY);

    if (bind (sock, (struct sockaddr *) &sockinfo, sizeof (sockinfo)) < 0)
    {
        perror ("bind");
        exit (EXIT_FAILURE);
    }

    log_inf("host %s, port %hd\n",
            inet_ntoa (sockinfo.sin_addr),
            ntohs (sockinfo.sin_port));

    if(serverStarted != NULL)
        serverStarted(sock, &sockinfo);

    log_dbg("end socket: %d", sock);
    return sock;
}

pthread_t makeClient(char* host, int port)
{
    log_dbg("begin host: %s port: %d", host, port);
    int sock;
    sock = createClientSocketAndConnect(host, port);
    pthread_t listenerThread; 
    pthread_create(&listenerThread, NULL, socketClientListener, &sock );
    log_dbg("end");
    return listenerThread;
}

int createClientSocketAndConnect(char* host, int port)
{
    log_dbg("begin");

    struct hostent *hp;
    struct sockaddr_in sin;
    int sock;
    /* fill in hostent struct */
    hp = gethostbyname(host); 
    if ( hp == NULL ) {
        fprintf(stderr, "%s: host not found \n", host);
        exit(1);
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if ( sock < 0 ) {
        perror("socket:");
        exit(sock);
    }

    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    memcpy(&sin.sin_addr, hp->h_addr_list[0], hp->h_length);

    /* connect to socket at above addr and port */
    int rc = connect(sock, (struct sockaddr *)&sin, sizeof(sin));
    if ( rc < 0 ) {
        perror("connect:");
        exit(rc);
    }

    if(serverConnected != NULL)
        serverConnected(sock, (struct sockaddr *)&sin);

    log_dbg("end connection successful");
    return sock;
}

struct sockaddr_in getPeerFromSocketFD(int sock)
{
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int res = getpeername(sock, (struct sockaddr *)&addr, &addr_size);
    return addr;
}
