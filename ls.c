//Course - Linux Systems Programming
//Assignment 1 - implemention of ls
//Date - Sept 12, 2017
//Author - Divyashree Ravindrakumar
//get_modebits - is taken from Rgahav vinjamuri's LSP example programs from Chapter2 Example 9k

#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <string.h>
#include <stdint.h>

typedef int bool;
#define true 1
#define false 0

static bool longlisting=false;

char *get_modebits(mode_t,char *);
int list(const char*);
void printpath(const char*, const char*);

bool is_file(const char* path) {
    struct stat buf;
    stat(path, &buf);
    return S_ISREG(buf.st_mode);
}

bool is_dir(const char* path) {
    struct stat buf;
    stat(path, &buf);
    return S_ISDIR(buf.st_mode);
}

int ls(int argc, char* argv[]) {
    char *curr_dir = NULL;
    unsigned int count = 0;
    if (argc == 1) {
//        printf("argc ==1 argv[0] %s\n",argv[0] );
        curr_dir = ".";
        list(".");
    }
    else{
        if(argv[1][0] == '-'){
            longlisting = true;
            if (argc == 2) {
                curr_dir = "./";
                list(curr_dir);
            } else if (argc > 2) {
		int i;
                for(i=2; i < argc; ++i) {
                    curr_dir = argv[i];
                    list(curr_dir);
                }
            }
        }
        else{
            if (argc >= 2) {
		    int i;
                for(i=1; i < argc; ++i) {
                    curr_dir = argv[i];
                    list(curr_dir);
                }
            }
        }
    }
    return 0;
}

int list(const char * name){
    DIR *dp = NULL;
    struct dirent *dirptr = NULL;
    if(name == NULL)
    {
        printf("\n ERROR : NULL directory name");
        return -1;
    }
//    printf("name %s\n", name);
    if (is_dir(name))
    {
        dp = opendir((const char*)name);
        if(dp == NULL)
        {
            printf("\n ERROR : Could not open %s\n", name);
            return -1;
        }
        else if (ENOENT == errno)
        {
            printf("\n ERROR : %s No such directory\n", name);
            return -1;
        }
        printf("%s\n", name);
	int i;
        for(i=0; (dirptr = readdir(dp)) != NULL ; i++)
        {
            // Check if the name of the file/folder begins with '.'
            // If yes, then do not display it.
            if(dirptr->d_name[0] != '.'){
//                printf("Printpath %s\n", dirptr->d_name);
                printpath(name, dirptr->d_name);
            }
        }
        closedir(dp);
    }
    else if(is_file(name)){
        if(name[0] != '.'){
//            printf("Printpath %s\n", dirptr->d_name);
            printpath(name, name);
        }
    }
    else{
        printf("ERROR : %s No such file or directory\n", name);
        return -1;
    }
    return 0;
}

void printpath(const char* dirname, const char* filename){
//    printf("long lisiting %d\n", longlisting);
//    printf("In printpath \n");
    struct stat statbuf;
    char buf[40];
    struct passwd  *pwd;
    struct group   *grp;
    struct tm      *tm;
    char            datestring[256];
    char path[2048];

    if (dirname != NULL) {
        strcpy(path, dirname);
        strcat(path, "/"); // just incase user did not end dirname with trailing slash
        strcat(path, filename);
    }

    if (stat(path, &statbuf) == -1){
//        printf("Returning!!!");
        return;
    }
    if (longlisting == true){
        /* Print out type, permissions, and number of links. */
        printf("%10.10s", get_modebits((mode_t)statbuf.st_mode,buf));
        printf("%4d", statbuf.st_nlink);

        /* Print out owner's name if it is found using getpwuid(). */
        if ((pwd = getpwuid(statbuf.st_uid)) != NULL)
            printf(" %-8.8s", pwd->pw_name);
        else
            printf(" %-8d", statbuf.st_uid);

        /* Print out group name if it is found using getgrgid(). */
        if ((grp = getgrgid(statbuf.st_gid)) != NULL)
            printf(" %-8.8s", grp->gr_name);
        else
            printf(" %-8d", statbuf.st_gid);

        /* Print size of file. */
        printf(" %9jd", (intmax_t)statbuf.st_size);

        tm = localtime(&statbuf.st_mtime);

        /* Get localized date string. */
        strftime(datestring, sizeof(datestring), nl_langinfo(D_T_FMT), tm);

        printf(" %s %s\n", datestring, filename);
    } else {
        printf("%s\n", filename);
    }
}

//This is taken from Raghav Vinjamuri's notes of LSP from chapter
char *get_modebits(mode_t modebits,char *buf) {
    int i=0;

    if      (S_ISDIR(modebits))         buf[i]='d';
    else if (S_ISFIFO(modebits))        buf[i]='p';
    else if (S_ISLNK(modebits))         buf[i]='l';
    else if (S_ISBLK(modebits))         buf[i]='b';
    else if (S_ISCHR(modebits))         buf[i]='c';
    else if (S_ISSOCK(modebits))        buf[i]='s';
    else if (S_ISREG(modebits))         buf[i]='-';
    else buf[i]='!';
    i++;
    buf[i++]=(modebits & S_IRUSR) ? 'r' : '-';
    buf[i++]=(modebits & S_IWUSR) ? 'w' : '-';
    buf[i++]=(modebits & S_IXUSR) ? 'x' : '-';

    buf[i++]=(modebits & S_IRGRP) ? 'r' : '-';
    buf[i++]=(modebits & S_IWGRP) ? 'w' : '-';
    buf[i++]=(modebits & S_IXGRP) ? 'x' : '-';

    buf[i++]=(modebits & S_IROTH) ? 'r' : '-';
    buf[i++]=(modebits & S_IWOTH) ? 'w' : '-';
    buf[i++]=(modebits & S_IXOTH) ? 'x' : '-';

    return buf;
}

//Sample Output
//TUSCA09UMLVT00I:assignment1 hasdi8d$ ./ls /Users/hasdi8d/Documents/LinuxSystemsProgramming
///Users/hasdi8d/Documents/LinuxSystemsProgramming
//assignments
//cmake-build-debug
//duedate.txt
//Lectures
//Notes

//TUSCA09UMLVT00I:assignment1 hasdi8d$ ./ls
//./
//a.out
//hw1.c
//ls
//ls.c
//ls_color.c
//Makefile
//new.c
//old.ls.c

//TUSCA09UMLVT00I:assignment1 hasdi8d$ ./ls -l
//./
//-rwxr-xr-x   1 hasdi8d  USWIN\Do     13564 Tue Sep 12 17:43:15 2017 a.out
//-rw-r--r--   1 hasdi8d  USWIN\Do      4999 Tue Sep 12 17:34:44 2017 hw1.c
//-rwxr-xr-x   1 hasdi8d  USWIN\Do     13564 Tue Sep 12 17:43:34 2017 ls
//-rw-r--r--   1 hasdi8d  USWIN\Do      6088 Tue Sep 12 17:41:57 2017 ls.c
//-rw-r--r--   1 hasdi8d  USWIN\Do      1853 Tue Sep 12 14:19:56 2017 ls_color.c
//-rw-r--r--   1 hasdi8d  USWIN\Do        71 Tue Sep 12 15:41:57 2017 Makefile
//-rw-r--r--   1 hasdi8d  USWIN\Do      6086 Tue Sep 12 17:40:42 2017 new.c
//-rw-r--r--   1 hasdi8d  USWIN\Do      2703 Tue Sep 12 17:40:24 2017 old.ls.c

//TUSCA09UMLVT00I:assignment1 hasdi8d$ ./ls -l /Users/hasdi8d/Documents/LinuxSystemsProgramming
///Users/hasdi8d/Documents/LinuxSystemsProgramming
//drwxr-xr-x   3 hasdi8d  USWIN\Do       102 Tue Sep 12 14:42:26 2017 assignments
//drwxr-xr-x   3 hasdi8d  USWIN\Do       102 Tue Aug  8 18:44:34 2017 cmake-build-debug
//-rw-r--r--   1 hasdi8d  USWIN\Do        41 Tue Aug  8 20:16:03 2017 duedate.txt
//drwxr-xr-x   7 hasdi8d  USWIN\Do       238 Mon Jul 17 20:12:22 2017 Lectures
//drwxr-xr-x  17 hasdi8d  USWIN\Do       578 Tue Sep 12 08:25:34 2017 Notes

//TUSCA09UMLVT00I:assignment1 hasdi8d$ ./ls -l "dodo"
//ERROR : dodo No such file or directory
