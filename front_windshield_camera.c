#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char** argv){
    printf("WFC opened and active \n");
    int fd = 0;

    FILE * fpd = fopen("frontCamera.data", "r");
    if(fpd == NULL)
        exit(EXIT_FAILURE);

    FILE * fpl = fopen("camera.log", "w");
    if(fpl == NULL)
        exit(EXIT_FAILURE);

    char * path5 = "/tmp/front";
    mkfifo(path5, 0666);


    char * line;
    line = (char*)malloc(sizeof(char)*9);
    size_t len = 0;

    while(1){
        sleep(10);
        if(getline(&line, &len, fpd) != -1){
            fd = open(path5, O_WRONLY);
            write(fd, line, 9);
            close(fd);
            fprintf(fpl, "%s", line);

        }else{
            line = "NO INSTRUCTION";
            write(fd, line, len);
            break;
        }
    }
    fclose(fpl);
    fclose(fpd);

    exit(EXIT_SUCCESS);
}
