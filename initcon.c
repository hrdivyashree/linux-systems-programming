#define _POSIX_SOURCE 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "initcon.h"

int inet_connect(const char* name, const char* port, int type)
{
    int sock;
    struct addrinfo hints;
    struct addrinfo* addrinfo;
    int rc;

    sock = -1;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = type;
    if (!name || !port) {
        fprintf(stderr, "(%s:%d) %s(), invalid args.\n", __FILE__, __LINE__, __FUNCTION__);
        sock = -1;
        return sock;
    }
    rc = getaddrinfo(name, port, &hints, &addrinfo);
    if (rc != 0) {
        fprintf(stderr, "(%s:%d) %s(), getaddrinfo() returned: %d,  %s\n", __FILE__, __LINE__, __FUNCTION__, rc, gai_strerror(rc));
        return -1;
    }
    struct addrinfo* iter;
    for (iter=addrinfo; iter!=NULL; ++iter) {

        sock = socket(iter->ai_family, iter->ai_socktype, iter->ai_protocol);
        if (sock < 0) continue;

        rc = connect(sock, iter->ai_addr, iter->ai_addrlen);
        if (rc < 0) {
            fprintf(stderr, "(%s:%d) %s(), connect() returned: %d\n", __FILE__, __LINE__, __FUNCTION__, rc);
            close(sock);
            sock = -1;
        } else {
            break;
        }
    }
    freeaddrinfo(addrinfo);
    return sock;
}

int inet_bind(const char* name, const char* port)
{
    int sock;
    struct addrinfo hints;
    struct addrinfo* addrinfo;
    int rc;

    sock = -1;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (!name || !port) {
        fprintf(stderr, "(%s:%d) %s(), invalid args.\n", __FILE__, __LINE__, __FUNCTION__);
        sock = -1;
        return sock;
    }
    rc = getaddrinfo(name, port, &hints, &addrinfo);
    if (rc != 0) {
        fprintf(stderr, "(%s:%d) %s(), getaddrinfo() returned: %d,  %s\n", __FILE__, __LINE__, __FUNCTION__, rc, gai_strerror(rc));
        return -1;
    }

    struct addrinfo* iter;
    for (iter=addrinfo; iter!=NULL; ++iter) {

        sock = socket(iter->ai_family, iter->ai_socktype, iter->ai_protocol);
        if (sock < 0) continue;

        rc = bind(sock, iter->ai_addr, iter->ai_addrlen);
        if (rc < 0) {
            fprintf(stderr, "(%s:%d) %s(), bind() returned: %d\n", __FILE__, __LINE__, __FUNCTION__, rc);
            close(sock);
            sock = -1;
        } else {
            struct sockaddr_in* sa = (struct sockaddr_in*)iter->ai_addr;
            const char* host_name = 0;
            int port_nr = 0;
            char* buf;
            int bufsize = 1024;
            buf = malloc(bufsize);
            if (buf) {
                memset(buf, 0, bufsize);
                host_name = inet_ntop(iter->ai_family, &sa->sin_addr, buf, bufsize);
                port_nr = ntohs(sa->sin_port);
                fprintf(stderr, "bind() success (%s:%d)\n", host_name, port_nr);
                free(buf);
            }
            break;
        }
    }

    freeaddrinfo(addrinfo);
    return sock;
}

int inet_listen(int sockfd, int backlog)
{
    int rc;
    rc = listen(sockfd, backlog);
    return rc;
}

int inet_accept(int sockfd)
{
    int peer;
    struct sockaddr_in peer_sa;
    socklen_t peer_sa_size = sizeof(struct sockaddr_in);
    peer = accept(sockfd, (struct sockaddr*)&peer_sa, &peer_sa_size);
    return peer;
}
