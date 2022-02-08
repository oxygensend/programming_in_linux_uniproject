#include "kolekcjoner.h"
#include <fcntl.h>
#include <errno.h>

// For singal handler
static volatile int numLiveChildren = 0;

int min(int x, int y){
	return (x > y) ? y : x;
}


int main(int argc, char ** argv){


   /* Dealing with flags */
    char c;
    while (( c = getopt(argc, argv, "d:s:w:f:l:p:")) != -1){
	
		if( optarg && optarg[0] == '-' && optarg[1] ){
			optopt = c + '\0';
			c = 63;
			optind--;
			}

		if(readFlags(c))
			return 11;
	}

    if(checkFlags()){
        return 11;
    }

   /* ----------------- */
	int fd_file_data = open(file_data, O_RDONLY);
	int fd_file_success = open(file_success, O_RDWR | O_TRUNC | O_CREAT , 0664 );
    int fd_file_raports = open(file_raports, O_WRONLY  | O_TRUNC | O_CREAT  , 0664);
	

   /* filling success file with null */
	pid_t p = 0;
	for(int i=0;i<MAX_SHORT;i++){
		write(fd_file_success,&p,sizeof(pid_t));
	}
	struct Record record;
	int status=0;
	int readfd[2], writefd[2];
	pipe(readfd);
	pipe(writefd);
	pid_t pid;
	pid_t child_pid[children_n];
	pid_t returned_pid;

	/* Linux Programming Interface chapter 44.9 */
	int flags = fcntl(readfd[0], F_GETFD);
	flags |= O_NONBLOCK;
	fcntl(readfd[0], F_SETFL, flags);
	
	
	/* ---------------------------------------- */

	double file_filled=0;	 
	int read_return;
	int read_bytes = 0;
	int written_bytes = 0;
	int read_buf = 1048;
	int buff;
	int closed=0;
	while(1){
		if(numLiveChildren < children_n){
			switch(pid=fork()){
				case -1: 
					write(fd_file_raports, "ERROR:Error occuried while fork()\n", 35);
				 	exit(1);
					break;
				case 0:
					childDo(fd_file_raports, readfd, writefd);	
					break;
				default:
					child_pid[numLiveChildren++] = pid;
					break;	
			}
		}
		else {
			
			/* Check if any process has termianated */
			if((returned_pid = waitpid(-1, &status, WNOHANG)) > 0){

				writeLogs(fd_file_raports,returned_pid,status);

				/* Jezli potomek konczy z błędem to zmieniejszamy liczbe potomkow*/
				if(WEXITSTATUS(status) > 10 || file_filled >= 0.75)
					children_n--;

				numLiveChildren--;
				continue;

			}
		    
			/* Check if read failed and no process terminated */
			if(returned_pid == 0 && read_return == -1 ){
				nsleep(0.48); 
			}

			/* If no process exists, exit program */
			if(returned_pid == -1)
				break;


			
			/* Read data from childs */
			if( (read_return=readData(readfd[0], 
				 fd_file_success,record)) != -1)
					read_bytes += read_return;

			file_filled = read_bytes/(double)(MAX_SHORT*2);
			

			/* Copy data from soruce to pipe */
			if( written_bytes < bytes_data_file){
				if( written_bytes + read_buf > bytes_data_file)
					buff = bytes_data_file - written_bytes;
				else 
					buff = read_buf;

				written_bytes += copyData(fd_file_data, writefd[1], buff);
			}
			/* Close buffor after wrting all data */
			if(written_bytes - bytes_data_file == 0 && closed == 0){
				closed = 1;
				close(writefd[1]);

			}
			
		}

		
	}

	printf("Bytes recived from child processes: %d\n", read_bytes);
	printf("Bytes readed  from  source and transferred to children: %d\n", written_bytes);
	printf("File was filled in %0.2lf%%\n", 100*file_filled);
	
			
}