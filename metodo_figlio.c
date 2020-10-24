/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   metodo_figlio.c
 * Author: brando
 *
 * Created on May 14, 2019, 11:13 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
/*
 * Si controlla qui che l' utente scriva Inizio e non una parola diversa. Di conseguenza, tale controllo non va fatto nella ECU.
 */
int main(int argc, char** argv) {


    int fd;
    char * myfifo = "/tmp/input";
    char buff[12];
    int control = 0;

    printf("Write Inizio for starting \n");

    //Ciclo per il comando inizio
    while(control == 0)
        {fgets(buff, sizeof(buff), stdin);
        if(strncmp(buff, "Inizio",6) == 0 || strncmp(buff, "inizio",6) == 0)
            control = 1;
        else
            {control = 0;
             printf("Non hai inserito una scelta valida. Riprova. \n");
            }
        }

    control = 0;
    fd = open(myfifo, O_RDWR);
    write(fd, buff, strlen(buff)+1);
    close(fd);

    return (EXIT_SUCCESS);
}
