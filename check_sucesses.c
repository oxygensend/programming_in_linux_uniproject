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


int main(int argc, char ** argv){
int fd_file_success = open(argv[1], O_RDONLY);

	pid_t x;
    int i=0;
	while(read(fd_file_success,&x, sizeof(pid_t))>0 ){
        i++;
            if(x!=0)
                printf("%d-%d\n", i, x);
	}
            
}	