#include "kolekcjoner.h"
#include <fcntl.h>
#include <errno.h>

// For singal handler
static volatile int numLiveChildren = 0;


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
	int fd_file_success_r = open(file_success, O_RDONLY | O_NONBLOCK);
	int fd_file_success_w = open(file_success, O_WRONLY | O_NONBLOCK);
    int fd_file_raports = open(file_raports, O_WRONLY | O_NONBLOCK | O_CREAT, 0664);
	
   /* filling success file with null */
	pid_t p = 0;
	for(int i=0;i<65536;i++){
		write(fd_file_success_w,&p,sizeof(pid_t));
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
	// flags = fcntl(writefd[1], F_GETFD);
	// flags |= O_NONBLOCK;
	// fcntl(writefd[1], F_SETFL, flags);
	// // flags = fcntl(writefd[0], F_GETFD);
	// // flags |= O_NONBLOCK;
	// // fcntl(writefd[0], F_SETFL, flags);
	
	/* ---------------------------------------- */

	double file_filled=0;	 
	int read_return;
	int written_data = 0;
	int readed_data = 0;
	int read_buf = 0;
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

			read_buf = 1000;	
			// printf("writen-%d readen-%d\n", written_data, readed_data);
			if( readed_data+read_buf < bytes_data_file ){
				// printf("hallo\n");
				readed_data += copyData(fd_file_data, writefd[1], read_buf);
			}
			else if( readed_data < bytes_data_file){
				// printf("halo\n");
				read_buf = bytes_data_file - readed_data;
				readed_data += copyData(fd_file_data, writefd[1], read_buf);
			}
			/* Close buffor after wrting all data */
			if(readed_data - bytes_data_file == 0)
				close(writefd[1]);

			/* Read data from childs */
			if( (read_return=readData(readfd[0], 
				 fd_file_success_r, 
				 fd_file_success_w,record)) != -1)
					written_data += read_return;

			file_filled = written_data/(double)MAX_SHORT;

			if((returned_pid = waitpid(-1, &status, WNOHANG)) > 0){

				writeLogs(fd_file_raports,returned_pid,status);

				/* Jezli potomek konczy z błędem to zmieniejszamy liczbe potomkow*/
				if(WEXITSTATUS(status) > 10 || file_filled >= 0.75)
					children_n--;

				updateActiveChildren(child_pid,numLiveChildren,returned_pid);
				numLiveChildren--;

				
			}

			if(numLiveChildren == 0)
				break;
			
			/* Check if both operations failed. */
			 if(returned_pid <= 0 && read_return == -1 )
			 	nsleep(0.48);

		}

		
	}

	printf("File was filled in %0.2lf%%\n", 100*file_filled);
	
			
}