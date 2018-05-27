//
// Created by Hassan Ravindrakumar, Divyashree (Divya) on 9/20/17.
//
//
// Created by Hassan Ravindrakumar, Divyashree (Divya) on 9/20/17.
// emulation of cat command using fgets, only input supported is files
// does not support any commandline options
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int cat(int argc, char** argv)
{
    int rc = 0;
    FILE* fp;
    char* buf = 0;
    int bufsize = 1024;
    char* filename;
    buf = malloc(bufsize);
    if (!buf) {
        printf("Could no allocate buffer");
        rc = -1;
        return rc;
    }

    int i=1;
    for (i=1; i<argc; ++i) {
        filename=argv[i];
        fp = fopen(filename, "r");
        if (!fp) {
            fprintf(stderr, "File %s open failed\n", filename);
            rc =-1;
        } else {
            while (fgets(buf, bufsize, fp)) {
                printf("%s", buf);
            }
            fclose(fp);
        }
    }
    return rc;
}

//int main(int argc, char** argv){
//    cat(argc, argv);
//}

