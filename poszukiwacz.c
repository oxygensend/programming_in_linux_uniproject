#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
struct Record{

    unsigned short    value;
    pid_t    process_pid;

}Record;



/* Function to get int from char */
int getInt(char *str){

    char * endptr;
    int x = strtol(str, &endptr,10);
    if(*endptr || x < 0){
        perror("Bad number format");
        return -1;
    }
    return x;

}

/* Function to get amount of numbers that have to be read */
int getAmountOfShorts(char *str){
    

    int n = strlen(str);
    int x;
    if(str[n-2] == 'M' && str[n-1] == 'i'){
        char subbuff[n-2];
        memcpy( subbuff, str, n-2);
        subbuff[n-2] = '\0';
        x = getInt(subbuff) * 1024 * 1024;
    }
    else if( str[n-2] == 'K' && str[n-1] == 'i'){
        char subbuff[n-2];
        memcpy( subbuff, str, n-2 );
        subbuff[n-2] = '\0';
        x = getInt(subbuff)  * 1024;
    }
    else {
        x = getInt(str);
    }
        
    return x;

}


/* Check if value is already in array */
int isInArray(unsigned short x, struct Record records [], int nSize){

    for(int i=0;i<nSize;i++){

        if(x == records[i].value)
            return 1;
    }
    return 0;
}

/* Returns value depending on the number of duplicates */
int returnValue(double duplicated){

    if(!duplicated)
        return 0;
    else if(duplicated >0 && duplicated <= 0.1)
        return 1;
    else if( duplicated > 1)
        return 10;

    for(double i=0.1;i<1;i+=0.1){
        if(duplicated > i && duplicated <= i+0.1)
            return i*10;
    }

}

int main(int argc, char **argv){

    if( argc != 2 ){

            perror("Program needs argument");
            return 11;
        }

    struct stat   buffer;   

    /* Check if standard input is connected to pipe */
    if(fstat(STDIN_FILENO, &buffer) == -1){
        perror("File doesnt exist");
        return 11;
    }
    if(!S_ISFIFO(buffer.st_mode)){
        perror("Standard input is not connected to pipe.");
        return 11;
    }

    int nSize = getAmountOfShorts(argv[1]);

    if(nSize == -1)
        return 12;

    struct Record records[nSize];
    for(int i=0;i<nSize;i++){
        records[i].value = -1;
        records[i].process_pid = 0;
    }

    unsigned short x;
    int j = 0;
    int bytes_read=0;
    int bytes = 0;
    while(bytes_read < nSize*2){
        
        bytes=read(STDIN_FILENO, &x, sizeof(unsigned short));
       
        if( bytes <= 0 && bytes_read == 0 )
            return 13;
        if(bytes <= 0)
            break;
        if( bytes > 0){
            bytes_read += bytes;
        }

        if(!isInArray(x,records, nSize)){
            records[j].value = x;
            records[j].process_pid = getpid();

            write(STDOUT_FILENO,&records[j], sizeof(struct Record));
            j++;
        }

    }

    // printf("halo-%d\n", j);
    double duplicated = (bytes_read - 2*j) / (double)bytes_read;

    // printf("status-%lf\n", duplicated);
    return(returnValue(duplicated));

}