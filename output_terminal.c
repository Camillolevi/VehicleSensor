#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char** argv) {
    printf("Opening output\n");
  FILE * fp;
  char * line;
  size_t len = 32;

  fp = fopen("ECU.log", "w+");
  if(fp == NULL)
    exit(EXIT_FAILURE);

  line = (char *)malloc(len*sizeof(char));

  while(1){

    getline(&line, &len, fp);

    if(feof(fp) > 0){
      clearerr(fp);
      sleep(1);
    }else{
      printf("%s", line);
      if(!strcmp(line, "FINE\n")){
        return (EXIT_SUCCESS);
      }
    }
  }

  fclose(fp);
  return (EXIT_SUCCESS);
}
