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



// Function to get int from char *
short getShort(char *str){

    char * endptr;
    short x = (short)strtol(str, &endptr,10);
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
        x = getShort(subbuff) * 1024 * 1024;
    }
    else if( str[strlen(str)-1] == 'K'){
        char subbuff[strlen(str)-1];
        memcpy( subbuff, str, strlen(str)-1 );
        x = getShort(subbuff)  * 1024;
    }
    else {
        x = getShort(str);
    }
        
    return x;


}


// Check if value is already in array
int isInArray(short x, struct Record* records){

    for(int i=0;i<sizeof(records);i++){
        if(x == records[i].value)
            return 1;
    }
    return 0;
}

// Returns value depending on the number of duplicates
int returnValue(int duplicated){

    if(!duplicated)
        return 0;
    else if(duplicated >=1 && duplicated <= 10)
        return 1;

    for(int i=1;i<10;i++){
        if(duplicated >= ( i*10 + 1) && duplicated <= (i+1)*10)
            return i;
    }

}

int main(int argc, char **argv){


 if( argc != 2 ){

            perror("Program needs argument");
            return 11;
        }



    int bytes = getBytes(argv[1]);
    int nSize = bytes/2;
    struct Record records[nSize];

   // mkfifo("test_fifo", 0777);
    int fd = open("test_fifo", O_RDONLY);
    if(fd == -1){
        perror("Failed to open fifo");
        return 11;
    }

    short x;
    int j = 0;
    dup2(fd,0);
        

    while(j<nSize){

        //short bytes_read = fread(&x, sizeof(short), 1, stdin);

        fflush(stdin);
        fscanf(stdin, "%hd", &x);
        
        records[j].duplicated = isInArray(x,records) ? 1 : 0;
        records[j].value = x;
        records[j].process_pid = getpid();

        j++;

    }
    

    int temp = 0;
    for(int i=0;i<nSize;i++){

        if(records[i].duplicated == 0){
            printf("%hd\t%d\n", records[i].value, records[i].process_pid);
            temp++;
        }
        
    }

    int duplicated = (nSize - temp);

    return(returnValue(duplicated));

}