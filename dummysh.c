//
// Created by Hassan Ravindrakumar, Divyashree (Divya) on 9/20/17.
// Referring to Raghav Vinjamuri's 7F example(poll) in chapter5 for timedIO
// Referring to Raghav Vinjamuri's example 8 from Chpater 5 for a dummy shell
// Using previous assignment functions ls , wc and cat for built in commands for shell simulation
//

#define DUMMY_PROMPT "dummysh> "
#define TMOUT 15

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <poll.h>

//int cat(int argc, char** argv);
//int wc(int argc, char** argv);
//int ls(int argc, char** argv);

void do_cmd(char *buf, int);
int parse_cmd(char *buf, char **vbuf, int);
int built_in_cmd(int argc, char** argv);
int process_cmd(char **argv);
int printwaitstatus(FILE *wfp, int pid, int st);


int printwaitstatus(FILE *wfp, int pid, int st) {

    fprintf(wfp,"\n");
    fprintf(wfp,"%6d=wait()",pid);
    if (WIFEXITED(st)) {
        fprintf(wfp,"exit: %3d\n", WEXITSTATUS(st));
    }
    else if (WIFSTOPPED(st)) {
        fprintf(wfp,"stop status: %3d\n", WSTOPSIG(st));
    }
    else if (WIFSIGNALED(st)) {
        fprintf(wfp,"termination signal: %3d\n", WTERMSIG(st));
        // fprintf(wfp,"\tcore dump: %s\n", WIFCORE(st) ? "yes" : "no");
    }
    return 0;
}

int built_in_cmd(int argc, char** argv){
    int st;
    if ((strcmp(*argv,"exit") == 0) ||
        (strcmp(*argv,"ex") == 0) ||
        (strcmp(*argv,"quit") == 0) ||
        (strcmp(*argv,"q") == 0) ) {
        exit(0);
    }
    else if (strcmp(*argv,"cd") ==0) {
        if ((argv[1]) && (st=chdir(argv[1])) != 0) {
            fprintf(stderr,"ERR: \"cd\" to '%s' failed! \n",
                    argv[1]);
            return -1;
        }
        return 1;
    }
    else if (strcmp(*argv,"hello") ==0) {
        fprintf(stderr,"\nHello! from process '%d'.\n",
                getpid());
        return 1;
    } else if (strncmp(argv[0], "ls", 2) == 0) {
        ls(argc, argv);
        return 1;
    } else if (strncmp(argv[0], "cat", 3) == 0) {
        cat(argc, argv);
        return 1;
    } else if (strncmp(argv[0], "wc", 2) == 0) {
        wc(argc, argv);
        return 1;
    }
    return 0;
}

int process_cmd(char **argv){
    pid_t pid;

    pid = fork();
    if (pid < 0) {
        fprintf(stderr, "fork() returned: %d\n", pid);
        exit(-1);
    } else if (0 == pid) {
        // child process
        int ec;
        ec = execvp(argv[0], argv);
        if (ec < 0) {
            fprintf(stderr, "execvp(%s) returned: %d\n", argv[0], ec);
            _exit(errno);  // terminate the child
        }
    } else {
        // parent process
        int st;
        waitpid(pid, &st, 0);
        // print_wait_status(stdout, pid, st);
    }
    return 0;
}


int parse_cmd(char* buf, char** vbuf, int n)
{
    int i = 0;
    const char* delim = " ,\t\n";
    char* token;
    size_t len;

    token = strtok(buf, delim);
    while (token && i < n) {
        len = strlen(token);
        vbuf[i] = malloc(len + 1);
        strcpy(vbuf[i], token); // happily, strcpy() copies the terminating zero too.
        token = strtok(NULL, delim);
        ++i;
    }

    vbuf[i] = 0; // it should already be the case.

    return i;
}


void do_cmd(char* buf, int bufsize)
{
    int i;
    char* vbuf[32];
    int arg_count;
    int max_arg_count = sizeof vbuf / sizeof vbuf[0];
    memset(vbuf, 0, sizeof vbuf);
    arg_count = parse_cmd(buf, vbuf, max_arg_count - 1);

    if (arg_count < 1) {
        ;
    } else if (built_in_cmd(arg_count, vbuf)) {
        ;
    } else {
        process_cmd(vbuf);
    }

    for (i=0; i<arg_count; ++i) {
        if (vbuf[i]) {
            free(vbuf[i]);
        }
    }
}

int main(int argc, char **argv) {
    int i;
    int st;
    int linenum=0;
    char *buf;
    FILE *rfp=stdin;
    int bufsize = 4096;
    int retval;
    char* prompt;

    struct pollfd pollfd[1];
    int myfd1=STDIN_FILENO;
    memset(pollfd, 0,sizeof(struct pollfd));
    pollfd->fd=myfd1;
    pollfd->events |= POLLIN;

//    fprintf(stderr,"Enter a char: (TIMEOUT=%d): ",TMOUT);
//    retval = poll(pollfd,1,TMOUT*1000); // 5s timeout
//     retval = poll(pollfd,1,-1); // -1 signifies infinite timeout

    buf = malloc(bufsize);
    if (!buf) {
        exit(-1);
    }

    if (isatty(myfd1)) {
        prompt = DUMMY_PROMPT;
        fprintf(stderr, "%s", prompt);
    }

    while (fgets(buf,bufsize,rfp))
    {
        if (prompt) {
            fprintf(stderr, "%s", prompt);
        }
        //TIMED IO using POLL
        retval = poll(pollfd, 1, TMOUT*1000);

        if (retval == 0) {
            fprintf(stderr, "No data within '%d' seconds.\n",TMOUT);
            continue;
        } else if (retval < 0) {
            perror("poll()");
            break;
        }

        memset(buf, 0, sizeof buf);
        retval = read(myfd1, buf, bufsize-1);

        if ((retval > 0 ) && *buf) {
            do_cmd(buf, bufsize);
        }

    }

    free(buf);
    return 0;
}
