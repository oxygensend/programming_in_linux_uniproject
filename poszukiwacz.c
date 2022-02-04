#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

struct Record{

    short    value;
    pid_t    process_pid;

}Record;



// Function to get int from char *
int getInt(char *str){

    char * endptr;
    int x = strtol(str, &endptr,10);
    if(*endptr || x < 0){
        perror("Bad number format");
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

    struct stat   buffer;   

    // Check if standard input is connected to pipe
    if(fstat(STDIN_FILENO, &buffer) == -1){
        perror("File doesnt exist");
        return 11;
    }
    if(!S_ISFIFO(buffer.st_mode)){
        perror("Standard input is not connected to pipe.");
        return 11;
    }

    short x;
    int nSize = getBytes(argv[1]);

    if(nSize == -1)
        return 12;
    struct Record records[nSize];
    int j = 0;


    for(int i=0;i<nSize;i++){
        //short bytes_read = fread(&x, sizeof(short), 1, stdin);

        // fflush(stdin);
        fscanf(stdin, "%hd", &x);
        
        if(!isInArray(x,records)){
            records[j].value = x;
            records[j].process_pid = getpid();

            j++;
        }

    }
    

    int temp = 0;
    for(int i=0;i<j;i++){
            // printf("%hd\t%d\n", records[i].value, records[i].process_pid);
            write(STDOUT_FILENO,&records[i], sizeof(struct Record));
            temp++;
    }
        
    

    int duplicated = (nSize - temp);

    return(returnValue(duplicated));

}