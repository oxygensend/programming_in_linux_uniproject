#ifndef KOLEKCJONER_H_
#define KOLEKCJONER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>

#define NANOSEC 1000000000L
#define FL2NANOSEC(f) {(long)(f), ((f)-(long)(f))*NANOSEC}
#define MAX_SHORT 65536


struct  Record{

    unsigned short    value;
    pid_t    process_pid;

}Record;



// flags definition
int flag_d;
int flag_s;
int flag_w;
int flag_f;
int flag_l;
int flag_p;

int bytes_data_file;
char * bytes_for_process;
int children_n;
char * file_success;
char * file_raports;
char * file_data;
struct stat   buffer;   


struct timespec tt;

// Function to get int from char *
int getInt(char * str);

// Function to get size of bytes that have to be read
int getBytes(char * str);
int readFlags(char c);
int checkFlags();
int copyData(int src, int dst, int buf_size);
void nsleep(float sec);
void childDo();
int writeSuccess(int offset, int fd_r, int fd_w, pid_t pid);
int readData(int fd, int fd_success_r, int fd_success_w, struct Record record);
void writeLogs(int fd_raports, pid_t returned_pid, int status);
void childDo(int fd_raports, int* readfd, int* writefd);
void updateActiveChildren(pid_t * pid_arr, int active_children,pid_t out_pid);

double checkSuccessesStatus(int fd);
// int writeData(int fd, pid_t process_pid, short value){

//     record.value = value;
//     record.process_pid = process_pid;

//    if(write(fd,&record, sizeof(struct Record)) == -1){
//        printf("Erorr occured while wirting data");
//        return 11;
//    }

// }

#endif