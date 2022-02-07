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


int main(){
int fd_file_success = open("osiagniecia.txt", O_RDONLY);

lseek(fd_file_success,0,SEEK_SET);
	pid_t x;
    int i=0;
	while(read(fd_file_success,&x, sizeof(pid_t))>0 ){
            
            if(x!=0)
                i++;
	}
    printf("%d\n", i);
            
}	