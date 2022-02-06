#include "kolekcjoner.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

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

    int n = strlen(str);
    int x;
    if(str[n-2] == 'M' && str[n-1] == 'i'){
        char subbuff[n-2];
        memcpy( subbuff, str, n-2);
        x = getInt(subbuff) * 1024 * 1024;
    }
    else if( str[n-2] == 'K' && str[n-1] == 'i'){
        char subbuff[n-2];
        memcpy( subbuff, str, n-2 );
        x = getInt(subbuff)  * 1024;
    }
    else {
        x = getInt(str);
    }
        
    return x;


}


int readFlags(char c){
		char *s;
		char * endptr;
		switch(c) {
		
			case 'd':
				flag_d = 1;
                file_data = optarg;
                if(stat(file_data, &buffer) == -1){
                    fprintf(stderr,"Check if file %s exists.\n", file_data);
                    return 1;
                }
                if(!S_ISREG(buffer.st_mode)){
                    fprintf(stderr,"File %s is not a regular file.", file_data);
                    return 1;
                }
				break;

			case 's':
				flag_s = 1;
                // strasznie dziwne rzeczy sie dzieja jak daje optarg bezposrednio do get Bytes
                s = optarg;
                if((bytes_data_file = getBytes(s)) == -1){
                    printf("xcd");
                    return 1;
                }
				break;

			case 'w':
				flag_w = 1;
                bytes_for_process = optarg; 
                
				break;

 			case 'f':
                flag_f = 1;
                file_success = optarg;
                if(stat(file_success, &buffer) == -1){
                    fprintf(stderr,"Check if file %s exists.\n", file_success);
                    return 1;
                }
                if(!S_ISREG(buffer.st_mode)){
                    fprintf(stderr,"File %s is not a regular file.", file_data);
                    return 1;
                }
				break;

 			case 'l':
                flag_l = 1;
                file_raports = optarg;
                if(stat(file_raports, &buffer) == -1){
                    fprintf(stderr,"Check if file %s exists.\n", file_raports);
                    return 1;
                }
                if(!S_ISREG(buffer.st_mode)){
                    fprintf(stderr,"File %s is not a regular file.", file_data);
                    return 1;
                }
 				break;

 			case 'p':
                flag_p = 1;
                if((children_n = getInt(optarg)) == -1)
                    return 1;
                break;
				
			case '?':
		
				if(optopt == 'd' || optopt == 's' || optopt == 'w' || optopt == 'f' || optopt == 'l' || optopt == 'p'){
					fprintf(stderr, "Option `-%c' requires an argument.\n", optopt);
					return 1;	
				}	
				else{ 
					fprintf(stderr, "Unknown option `-%c'.\n", optopt);
					return 1;
				}
		}
		
		return 0;

}

// Check if all flags are loaded
int checkFlags(){

    if(flag_l + flag_p + flag_d + flag_s + flag_f + flag_w < 6){
        perror("Flags are missing!\nRequired flags are: l,s,w,f,d,p");
        return 1;
    }

    return 0;
}

int copyData(int src_fd, int dst_fd){

    FILE* fp_src = fdopen(src_fd, "r");
    FILE* fp_dst = fdopen(dst_fd, "w");
    short x;
    int i=0;
    while(i<bytes_data_file){
        fscanf(fp_src,"%hd", &x);
        fprintf(fp_dst, "%hd ", x);
        i++;
    }
    
}

int readData(int fd){


}


char * createFormatedStr(char * s, int param, int size){


}
void nsleep(float sec){

    struct timespec sleep_t = FL2NANOSEC(sec);

    nanosleep(&sleep_t, NULL);


}




