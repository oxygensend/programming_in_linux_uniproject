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
#include <stdarg.h>
#include <sys/ioctl.h>
#define NANOSEC 1000000000L
#define FL2NANOSEC(f) {(long)(f), ((f)-(long)(f))*NANOSEC}
#define MAX_SHORT 65535

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

// for flags values
int bytes_data_file;
int bytes_process;
char * bytes_for_process;
char * file_success;
char * file_raports;
char * file_data;
struct stat   buffer;   

// global variables definition
int numLiveChildren ;
int children_n;
double file_filled;	 
struct timespec tt;


// function declaration
int getInt(char * str);
int getBytes(char * str);
int readFlags(char c);
int checkFlags();
int copyData(int src, int dst, int buf_size);
void nsleep(float sec);
void childDo();
int writeSuccess(int offset, int fd, pid_t pid);
int readData(int fd, int fd_success, struct Record record);
void writeLogs(int fd_raports, char * fnt, ... );
void childDo(int fd_raports, int* readfd, int* writefd);
int checkStatus(int fd);


#endif