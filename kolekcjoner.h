#ifndef KOLEKCJONER_H_
#define KOLEKCJONER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>



// flags definition

 int flag_d;
 int flag_s;
 int flag_w;
 int flag_f;
 int flag_l;
 int flag_p;

// Function to get int from char *
short getShort(char * str);

// Function to get size of bytes that have to be read
int getBytes(char * str);
int readFlags(char c);
int checkFlags();



// int writeData(int fd, pid_t process_pid, short value){

//     record.value = value;
//     record.process_pid = process_pid;

//    if(write(fd,&record, sizeof(struct Record)) == -1){
//        printf("Erorr occured while wirting data");
//        return 11;
//    }

// }

#endif