#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int invio(FILE *fdr);

int main(int argc, char** argv) {

  printf("Park assist opened and active \n");
  FILE *fdr;

  if (strcmp(argv[1], "1") == 0)
  fdr = fopen("/dev/urandom", "r");
  else
  fdr = fopen("urandomARTIFICIALE.binary", "rb");

  invio(fdr);

  return(EXIT_SUCCESS);
}

int invio(FILE *fdr){

  char * path6 = "/tmp/assist";
  uint * buff;
  FILE *fpl = fopen("assist.log", "w");
  int fdp;

  buff = (uint *)malloc(4*sizeof(uint));
  for(int i=0; i<30; i++){
    // Leggo i dati
    fread(buff, 4, 1, fdr);


    //Apro la pipe ed invio ciò che ho lettochar
    fdp = open(path6, O_WRONLY);
    write(fdp, buff, 4);
    close(fdp);
    //Scrivo nel log
    fwrite(buff, 1, 4, fpl);
    //Aspetto 1 secondo
    sleep(1);
  }

  return(EXIT_SUCCESS);
}
