/**
 * mostly copied from https://www.cs.cmu.edu/afs/cs/academic/class/15213-f99/www/class26/tcpserver.c
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string>
#include "loguru.hpp"

/*
 * error - wrapper for perror
 */
void error(const std::string &msg) {
    LOG_F(ERROR, "%s: %s", msg.c_str(), strerror(errno));
    exit(1);
}

void start_server(int port, void (*handler)(int)) {
    int parentfd; /* parent socket */
    int childfd; /* child socket */
    int portno = port; /* port to listen on */
    unsigned int clientlen; /* byte size of client's address */
    struct sockaddr_in serveraddr; /* server's addr */
    struct sockaddr_in clientaddr; /* client addr */
    struct hostent *hostp; /* client host info */
    char *hostaddrp; /* dotted decimal host addr string */
    int optval; /* flag value for setsockopt */
    /*
     * socket: create the parent socket
     */
    parentfd = socket(AF_INET, SOCK_STREAM, 0);
    if (parentfd < 0)
        error("ERROR opening socket");

    /* setsockopt: Handy debugging trick that lets
     * us rerun the server immediately after we kill it;
     * otherwise we have to wait about 20 secs.
     * Eliminates "ERROR on binding: Address already in use" error.
     */
    optval = 1;
    setsockopt(parentfd, SOL_SOCKET, SO_REUSEADDR,
               (const void *) &optval, sizeof(int));

    /*
     * build the server's Internet address
     */
    bzero((char *) &serveraddr, sizeof(serveraddr));

    /* this is an Internet address */
    serveraddr.sin_family = AF_INET;

    /* let the system figure out our IP address */
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* this is the port we will listen on */
    serveraddr.sin_port = htons((unsigned short) portno);

    /*
     * bind: associate the parent socket with a port
     */
    if (bind(parentfd, (struct sockaddr *) &serveraddr,
             sizeof(serveraddr)) < 0)
        error("ERROR on binding");

    /*
     * listen: make this socket ready to accept connection requests
     */
    if (listen(parentfd, 5) < 0) /* allow 5 requests to queue up */
        error("ERROR on listen");

    LOG_F(INFO, "listening on port %d", portno);

    /*
     * main loop: wait for a connection request, echo input line,
     * then close connection.
     */
    clientlen = sizeof(clientaddr);
    while (1) {

        /*
         * accept: wait for a connection request
         */
        childfd = accept(parentfd, (struct sockaddr *) &clientaddr, &clientlen);
        if (childfd < 0)
            error("ERROR on accept");

        /*
         * gethostbyaddr: determine who sent the message
         */
        hostp = gethostbyaddr((const char *) &clientaddr.sin_addr.s_addr,
                              sizeof(clientaddr.sin_addr.s_addr), AF_INET);
        if (hostp == NULL)
            error("ERROR on gethostbyaddr");
        hostaddrp = inet_ntoa(clientaddr.sin_addr);
        if (hostaddrp == NULL)
            error("ERROR on inet_ntoa\n");

        handler(childfd);
        close(childfd);
    }
}
