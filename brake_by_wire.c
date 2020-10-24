#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

int flag = 1;

void emergencyHandler(int sig);
void stopHandler(int sig);

int main(int argc, char** argv){

  printf("BBW opened and active \n");
  char * myfifo = "/tmp/brake";
  int fd;
  int pid;
  int i;

  void (*prev_sighandler)(int);
  prev_sighandler = signal(SIGUSR1, emergencyHandler);

  void (*prev_sighandler2)(int);
  prev_sighandler2 = signal(SIGUSR2, stopHandler);

  FILE * fp = fopen("brake.log", "w");
  if(fp == NULL){
    exit(EXIT_FAILURE);
  }

  char cmd[9];
  char tmp[2];
  int dec;
  int ctrl = 0;

  pid = fork();
  switch(pid){
    case -1:
    perror("BBW: Fork error");
    break;
    case 0:
    while(1){
      fwrite("NO ACTION\n", 1, sizeof("NO ACTION\n")-1, fp);
      sleep(1);
    }
  }

  while(1){

    fd = open(myfifo, O_RDONLY);
    ctrl = read(fd, cmd, 9);
    kill(pid, SIGSTOP);
    close(fd);

    if(ctrl == 0)
      printf("BBW: Error. I've read 0 character \n");

    if(cmd[7] != '\0'){
      tmp[0] = cmd[6];
      tmp[1] = cmd[7];
      dec = atoi(tmp);
    }

    i = dec;
    printf("BBW: Decreasing speed \n");
    while(flag && i > 0){
      fwrite("DECREMENTO 5\n", 1, sizeof("DECREMENTO 5\n")-1, fp);
      sleep(1);
      i = i - 5;
    }
    flag = 1;
    kill(pid, SIGCONT);
  }

  fclose(fp);
  exit(EXIT_SUCCESS);
}

void emergencyHandler(int sig){

  if (sig == SIGUSR1){
    printf("BBW: Managing emergency brake\n");
    signal(sig, SIG_IGN);
    flag = 0;
    FILE * efp = fopen("brake.log", "a");
    fwrite("ARRESTO AUTO\n", 1, sizeof("ARRESTO AUTO\n")-1, efp);
    fclose(efp);
  } else {
    signal(sig, SIG_DFL);
  }

  signal(sig, emergencyHandler);
}

void stopHandler(int sig){

printf("Handler of BBW for parking \n");
  if (sig == SIGUSR2){
    signal(sig, SIG_IGN);

    FILE * efp = fopen("brake.log", "a");
    fwrite("PARCHEGGIO\n", 1, sizeof("PARCHEGGIO\n")-1, efp);
    fclose(efp);
  } else {
    signal(sig, SIG_DFL);
  }

  signal(sig, stopHandler);
}
