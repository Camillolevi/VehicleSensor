/*
* To change this license header, choose License Headers in Project Properties.
* To change this template file, choose Tools | Templates
* and open the template in the editor.
*/

/*
* File:   main.c
* Author: brando
*
* Created on May 7, 2019, 5:21 PM
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>


void controlloInputP(int dec, pid_t pid);
int parcheggio(pid_t pidb,pid_t pidf);

/*
* path1: percorso named pipe del processo per l' input
* path2: percorso named pipe del processo per steer by wire
* path3: percorso named pipe del processo per throttle_control
* path4: percorso named pipe del processo per brake by wire
* path5: percorso named pipe del processo per forward_facing_radar
*
*/

int main(int argc, char** argv) {

  /*Imposta la velocità a 0 e setta parametri utili per dopo*/
  int speed = 0;
  pid_t pid = getpid();
  printf("Speed at 0 \n");
  FILE *ecu;
  ecu = fopen("ECU.log", "w");



  int destRead = 0; // 0 ho inserito normale, 1 ho inserito artificiale.
  int fd = 0;
  int fd1 = 0;
  int fd2 = 0;
  int fd3 = 0;
  int fd4 = 0;
  pid_t pidb;
  FILE *file;

  char * path1 = "/tmp/input";
  char * path2 = "/tmp/steer";
  char * path3 = "/tmp/throttle";
  char * path4 = "/tmp/brake";
  char * path5 = "/tmp/front";


  destRead = atoi(argv[1]);


  mkfifo(path1, 0666);
  mkfifo(path2, 0666);
  mkfifo(path3, 0666);
  mkfifo(path4, 0666);
  mkfifo(path5, 0666);


  char buf[12];

  char *args[] = {"./metodo_figlio", NULL};
  char *args1[] = {"./steer", NULL};
  char *args2[] = {"./front", NULL};
  char *args3[] = {"./brake", NULL};
  char *args4[] = {"./throttle", NULL};
  // execvp(args[0],args);

  /*
  * Fork nella switch.
  * id < 0 errore, id = 0 lo legge il figlio, id = numero lo legge il padre padre.
  * Nel caso 0, esegue una execvo che costringe il figlio ad eseguire il file ./metodo_figlio
  * Il file in questione permette la scrittura di "Inizio" su terminale.
  */

  switch(fork()){
    case -1:
    perror("Errore nella prima fork \n");
    break;

    case 0:
    printf("Creating HMI...\n");
    execvp(args[0],args);
    return 0;
  }

  /*
  * Mentre il figlio permette la scrittura a tastiera di "Inizio", il padre si blocca in attesa del messaggio e del segnale
  * passato attraverso la named pipe.
  * La named pipe blocca il processo al comando read, in attesa della write nel metodo figlio (vedi funzionamento delle named pipe).
  */

  fd = open(path1, O_RDWR);
  read(fd, buf, 12);
  close(fd);

  //fork per brake

  pidb = fork();
  if(pidb == -1)
  {
    perror("Errore nella fork di brake");
  }
  if(pidb == 0){
    printf("Creating Brake by Wire\n");
    execvp(args3[0],args3);
    return 0;
  }
  /*
  * Nuova fork, strutturalmente uguale alla precedente, che crea un nuovo figlio. Il nuovo figlio andrà ad attivare il metodo
  * "controlloInput", che permette all' utente la scrittura da terminale di "parcheggio". Quando l' utente scrive parcheggio
  * il metodo manda una signal che blocca il processo padre, e attiva un secondo metodo relativo alle procedure di parcheggio (parcheggio()).
  */
  switch(fork()){

    case -1:
    perror("Errore nella fork di controllo input");
    break;
    case 0:
    controlloInputP(destRead, pidb);
    return (EXIT_SUCCESS);

  }

  /*
  *Nuova fork, per creare un figlio che gestisca il sensore steer_by_wire
  *
  */

  switch(fork()){

    case -1:
    perror("Errore nella fork di steer");
    break;
    case 0:
    printf("Creating Steer_by_wire \n");
    execvp(args1[0],args1);
    return 0;
  }


  switch(fork()){

    case -1:
    perror("Errore nella fork di front");
    break;
    case 0:
    printf("Creating frontWindShieldCamera \n");
    execvp(args2[0],args2);
    return 0;
  }


  switch(fork()){

    case -1:
    perror("Errore nella fork di throttle");
    break;
    case 0:
    printf("Creating Throttle Control\n");
    execvp(args4[0],args4);
    return 0;
  }
  printf("Operations... \n");

  while(1){
    printf("ECU: Speed is %d \n", speed);
    int i= 0;
    int fd6 = 0;
    char bff[9];
    int ctrl = 0;
    char vel[2];
    char str3[] = "DESTRA";
    char str4[] = "SINISTRA";
    char str5[] = "PERICOLO";
    int rq = 0;

    //Lettura FWC
    fd = open(path5, O_RDONLY);
    ctrl = read(fd, bff, 9);
    if(bff[0] == '\0' || ctrl == 0){
      close(fd);
      printf("ECU: Read empty string, nothing is done \n");
    }
    else{
      close(fd);
      printf("ECU: I've read: %s\n", bff);
      fwrite(bff, 1, 9, ecu);
      fflush(ecu);
    }
    //Controllo PERICOLO
    if(strncmp(bff,str5,8) == 0)
      {
            printf("Receiving PERICOLO. Initialiting speed at 0. Sending signal to brake.\n");
            speed = 0;
            kill(pidb, SIGUSR1);
        }
    //Controllo destra/sinistra
    else if(strncmp(bff,str3,6) == 0)
    {
      fd2 = open(path2, O_WRONLY);
      write(fd2, str3, strlen(str3)+1);
      close(fd2);}
      else if(strncmp(bff,str4,8) == 0)
      {
        fd2 = open(path2, O_WRONLY);
        write(fd2, str4, strlen(str4)+1);
        close(fd2);
      }
      //caso coi numeri
      else{
        rq =((int)(bff[1])-48)+((int)(bff[0])-48)*10;
        if(rq < speed){

          sprintf(vel, "%d", speed-rq);
          char str2[] = "FRENO XX";
          str2[6] = vel[0];
          str2[7] = vel[1];
          fd4 = open(path4, O_WRONLY);
          write(fd4, str2, strlen(str2)+1);
          close(fd4);
        }
        else if(rq > speed)
        {

          char str[] = "INCREMENTO XX";
          sprintf(vel, "%d", rq-speed);
          str[11] = vel[0];
          str[12] = vel[1];
          fd3 = open(path3, O_WRONLY);
          write(fd3, str, strlen(str)+1);
          close(fd3);
        }
        else{
          printf("ECU: The speed will remain constant \n");
        }
        speed = rq;
      }
    }
fclose(ecu);
return 0;
  }




  /*Metodo che controlla se hai digitato parcheggio*/

  void controlloInputP( int dec, pid_t pidb){
    sleep(5);
    pid_t pf = getppid();
    char *args5[] = {"./park_assist", "1", NULL};

    char buff[12];
    pid_t pidf;
    int control = 0;
    printf("%d: Write parcheggio, if you want to park the car. \n", getpid());

    while(control == 0){
      fgets(buff, sizeof(buff), stdin);
      if(strncmp(buff,"Parcheggio",10) == 0 || strncmp(buff,"parcheggio",10) == 0)
      control = 1;
      else{
        control = 0;
        printf("Not the correct input.\n");}
      }



      kill(pf, SIGSTOP);
      printf("Active parking operations \n");

      if(dec == 2){
        char *args5[] = {"./park_assist 2", NULL};}

      pidf = fork();
      switch(pidf){
         case -1:
         perror("Errore nella fork di park assist\n");
         break;
         case 0:
         printf("Creating park_assist \n");
         execvp(args5[0],args5);
         return;
       }


      int flag = 1;
      while(1){
        flag = parcheggio(pidb, pidf);
        printf("ECU: Control if parking have the correct input \n");
        if (flag == 1)
          break;
      }

      printf("Terminating the program.. \n");
      kill(pidf, SIGKILL);
      kill(0, SIGKILL);

    }

    //Metodo che finalizza il parcheggio
    int parcheggio(pid_t pidb, pid_t pidf){
      // Flag per indicare il successo
      int flag = 1;
      char * path6 = "/tmp/assist";
      mkfifo(path6, 0666);
      int ctrl = 0;

      int fd = 0;

      uint bf[6];

      printf("In parking method \n");
      //Invio segnale a BBW
      kill(pidb, SIGUSR2);

      for(int i = 0; i < 30; i++){
        fd = open(path6, O_RDONLY);
        ctrl = read(fd, bf, 4);
        if(ctrl == 0)
          printf("Error from the number of character from read \n");
        printf("ECU: I've read: %x\n", bf[0]);
        close(fd);
        printf("ECU: Confronting string\n");
        if ((bf[0] == 0x17 && bf[1] == 0x2A) || (bf[0] == 0xD6 && bf[1] == 0x93) || (bf[0] == 0xBD && bf[1] == 0xD8) || (bf[0] == 0xFA && bf[1] == 0xEE) || (bf[0] == 0x43 && bf[1] == 0x00)){
          // Fallimento
          ("ECU: Found the correct scring. Reactivating park() \n");
          flag = 0;
          return flag;
        }
        //Confronto seconda metà del buffer passato
        else if((bf[2] == 0x17 && bf[3] == 0x2A) || (bf[2] == 0xD6 && bf[3] == 0x93) || (bf[2] == 0xBD && bf[3] == 0xD8) || (bf[2] == 0xFA && bf[3] == 0xEE) || (bf[2] == 0x43 && bf[3] == 0x00))
        {
          ("ECU: Found the correct scring. Reactivating park() \n");
        flag = 0;
        return flag;
      }
        printf("ECU: Not the correct string. Closing ECU. \n");
        sleep(1);

      }
return flag;
    }
