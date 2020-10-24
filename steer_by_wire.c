#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

int main(int argc, char** argv) {

  printf("SBW opened and active\n");

  char * myfifo = "/tmp/steer";

  char buff[9];
  int i=0;
  int fd=0;
  int pid;
  FILE * fp = fopen("steer.log","w");
  int ctrl = 0;

  pid = fork();

  if (pid == 0) {

    while(1){
      fwrite("NO ACTION\n", 1, sizeof("NO ACTION\n")-1, fp);
      fflush(fp);
      sleep(1);
    }
  }
  while(1){
    fd=open(myfifo,O_RDONLY);
    kill(pid, SIGKILL);
    ctrl = read(fd,buff,9);
    close(fd);
    if(ctrl == 0)
    printf("SBW: Error. I've read 0 character \n");

    if (strcmp(buff,"DESTRA") == 0) {
      i=0;
      printf("SBW: Turning right for 4 seconds");
      for(i; i<4; i++) {
        fwrite("STO GIRANDO A DESTRA\n",1,sizeof("STO GIRANDO A DESTRA\n")-1,fp);
        fflush(fp);
        sleep(1);
      }
    }else if (strcmp(buff,"SINISTRA") == 0) {
      i=0;
      printf("SBW: Turning left for 4 seconds \n");
      for(i; i<4; i++) {
        fwrite("STO GIRANDO A SINISTRA\n",1,sizeof("STO GIRANDO A SINISTRA\n")-1,fp);
        fflush(fp);
        sleep(1);
      }
    }
  }
  return (EXIT_SUCCESS);
}
