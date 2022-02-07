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

int copyData(int src_fd, int dst_fd, int buf_size){

    char buf[buf_size];
    read(src_fd, &buf, 10);
    write(dst_fd, &buf,10 );

    return buf_size;

}

int readData(int fd, int fd_success_r, int fd_success_w, struct Record record){
    int i=0;
    int nr=0;
    while(1){
	    nr=read(fd,&record, sizeof(struct Record));
		if(nr == 0){
            break;
		}
		else if(nr == -1){
           break;	}
		else{
			 printf("%d\t%d\n", record.value, record.process_pid);

			i += writeSuccess(record.value*sizeof(pid_t), 
						fd_success_r, 
						fd_success_w,
						record.process_pid
						);
		}
	}

    return (nr == -1 && i == 0) ? -1 : i;
}


void nsleep(float sec){

    struct timespec sleep_t = FL2NANOSEC(sec);
    nanosleep(&sleep_t, NULL);

}


int writeSuccess(int offset, int fd_r, int fd_w, pid_t pid){

    pid_t temp;
    int success=0;
    lseek(fd_r,offset,SEEK_SET);
    read(fd_r,&temp, sizeof(pid_t));
    lseek(fd_w,offset,SEEK_SET);
    if(!temp){
        write(fd_w, &pid ,sizeof(pid_t));
        success=1;
    }
    return success;
}

void writeLogs(int fd_raports, pid_t returned_pid, int status){

    char * fnt = "EXIT:Process %d terminated with status - %d at %ld.%ld\n"; 
    char tab[100];
    clock_gettime(CLOCK_MONOTONIC, &tt);
    snprintf(tab, sizeof(tab), fnt,
    returned_pid, WEXITSTATUS(status), tt.tv_sec, tt.tv_nsec);
    write(fd_raports, tab, strlen(tab));
}

void childDo(int fd_raports, int * readfd, int * writefd){

    char * fnt = "CREATE:New process created pid = %d at %ld.%ld \n";
	char tab[100];

    clock_gettime(CLOCK_MONOTONIC, &tt);
    snprintf(tab,sizeof(tab), fnt, getpid(), tt.tv_sec, tt.tv_nsec);
    write(fd_raports, &tab, strlen(tab));
    dup2(readfd[1], STDOUT_FILENO);
    close(writefd[1]);
    close(readfd[1]);
    dup2(writefd[0], STDIN_FILENO);
    close(readfd[0]);
    close(writefd[0]);

    execl("poszukiwacz", "poszukiwacz",bytes_for_process, NULL);
}

void updateActiveChildren(pid_t * pid_arr, int active_children,pid_t out_pid){

    int index=0;
    for(int i=0;i<active_children;i++){
        if(pid_arr[i] == out_pid){index=i;break;}

    }
    pid_arr[index] = pid_arr[--active_children];


}

double checkSuccessesStatus(int fd){
    
    int count=0;
    pid_t pid;

    while(read(fd, &pid, sizeof(pid_t))>0){
        if(pid != 0)
            count++;
    }

    return count;
}