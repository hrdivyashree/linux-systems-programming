#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "parse.h"
#include "myshell.h"
#include "initcon.h"

int main(int argc, char** argv)
{
    struct params* params = 0;
    int sockfd = -1;
    params = alloc_params();
    extract_params_from_cmdline_options(params, argc, argv);
    sockfd = inet_connect(hostname(params), portnumber(params), SOCK_STREAM);
    if (sockfd < 0) {
        fprintf(stderr, "Failed to connect to remote host.\n");
        if (sockfd > 0) {
            close(sockfd);
        }
        free_params(params);
        return -1;
    }
    do_interactive_loop(sockfd);
    if (sockfd > 0) {
        close(sockfd);
    }
    free_params(params);
    return 0;
}
