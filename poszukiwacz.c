#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
struct __attribute__((__packed__)) Record{

    short    value;
    pid_t    process_pid;
    int      duplicated;

}Record;


struct Record record;

// Function to get int from char *
short getInt(char *str){

    char * endptr;
    int x = strtol(str, &endptr,10);
    if(*endptr){
        perror("Bad number format, gives 2 bytes number");
        return -1;
    }
    return x;

}

// Function to get size of bytes that have to be read
int getBytes(char *str){

    int x;
    if(str[strlen(str)-1] == 'M'){
        char subbuff[strlen(str)-1];
        memcpy( subbuff, str, strlen(str)-1 );
        x = getInt(subbuff) * 1024 * 1024;
    }
    else if( str[strlen(str)-1] == 'K'){
        char subbuff[strlen(str)-1];
        memcpy( subbuff, str, strlen(str)-1 );
        x = getInt(subbuff)  * 1024;
    }
    else {
        x = getInt(str);
    }
        
    return x;


}


int writeData(int fd, pid_t process_pid, short value){

    record.value = value;
    record.process_pid = process_pid;

   if(write(fd,&record, sizeof(struct Record)) == -1){
       printf("Erorr occured while wirting data");
       return 11;
   } error: array size missing in ‘records’

}

int isInArray(short x, struct Record* records){

    for(int i=0;i<sizeof(records);i++){
        if(x == records[i].value)
            return 1;
    }
    return 0;
}

int main(int argc, char **argv){


 // if( argc != 2 ){

        //     perror("Program needs argument");
        //     return(11);
        // }



    int bytes = getBytes("4");

    struct Record records[bytes/2];

   // mkfifo("test_fifo", 0777);
    while(1){
        int fd = open("test_fifo", O_RDONLY);
        if(fd == -1){
            perror("Failed to open fifo");
        }
        else {

            short x;
            int i = 0;
            dup2(fd,0);
           


                int bytes_read = fread(&x, sizeof(int), 1, stdin);
                fscanf(stdin, "%hd", &x);
                
                printf("b-%d r-%d\n", bytes_read, 1
                );
                printf("%d\n",x);
                i++;


                record.value = x;
                record.process_pid = getpid();
                record.duplicated = isInArray(x,records) ? 1 : 0;
                records[i] = record;



        }
    }
}