
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <string.h>

/*
*
*/
int main(int argc, char** argv) {

  printf("Throttle opened and active\n");
  char * myfifo = "/tmp/throttle";
  char buff[14];
  int speed=0;
  int pid = 0;
  int l=0;
  int b=0;
  char little[1];
  char big[2];
  int fd=0;
  FILE * fp = fopen("throttle.log","w");
  int ctrl = 0;
  pid = fork();

  if(pid == -1)
  {printf("THR: Error in fork creation \n");
  return 0;
}
if (pid == 0) {
  while(1){

    fwrite("NO ACTION\n", 1, sizeof("NO ACTION\n")-1, fp);
    fflush(fp);
    sleep(1);
  }
  return 0;
}

while (1) {
  fd=open(myfifo,O_RDONLY);
  ctrl = read(fd,buff,14);
  kill(pid, SIGKILL);
  close(fd);

  if(ctrl == 0)
    printf("THR: Error. I've read 0 character \n");

  int speed=0;
  big[0]=buff[11];
  big[1]=buff[12];

  b=((int)(big[1])-48)+((int)(big[0])-48)*10;
  printf("THR: Increasing speed \n");
  for(speed=0; speed<b; speed=speed+5) {
    fwrite("AUMENTO 5\n",1,sizeof("AUMENTO 5\n")-1,fp);
    fflush(fp);
    sleep(1);
  }

  kill(pid, SIGCONT);

}


return (EXIT_SUCCESS);
}
