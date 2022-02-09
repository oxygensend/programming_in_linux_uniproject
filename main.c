#include "kolekcjoner.h"



void errorExit( char * msg){

	perror(msg);
	exit(EXIT_FAILURE);

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
			exit(EXIT_FAILURE);
	}

    if(checkFlags()){
		errorExit("Flags are missing!\nRequired flags are: l,s,w,f,d,p");
    }

   /* ----------------- */
	int fd_file_data = open(file_data, O_RDONLY);
	int fd_file_success = open(file_success, O_RDWR | O_TRUNC | O_CREAT , 0664 );
    int fd_file_raports = open(file_raports, O_WRONLY  | O_TRUNC | O_CREAT  , 0664);
	
 
    /* Check if volume is within file size */
	if(fstat(fd_file_data, &buffer) == -1)
		exit(EXIT_FAILURE);
	
	if(buffer.st_size < bytes_data_file){
		fprintf(stderr,"Volume is bigger than source file size(%ld)\n", buffer.st_size);
		exit(EXIT_FAILURE);
	}

   /* filling success file with null */
	pid_t p = 0;
	for(int i=0;i<MAX_SHORT+1;i++){
		write(fd_file_success,&p,sizeof(pid_t));
	}

	/* inicialize global variables */
	numLiveChildren = 0;
	file_filled = 0;

	struct Record record;
	pid_t returned_pid;
	int readfd[2], writefd[2];
	pipe(readfd);
	pipe(writefd);

	/* Linux Programming Interface chapter 44.9 */
	int flags = fcntl(readfd[0], F_GETFD);
	flags |= O_NONBLOCK;
	fcntl(readfd[0], F_SETFL, flags);
	/* ---------------------------------------- */

	int read_return = 0;
	int read_bytes = 0;
	int written_bytes = 0;
	int read_buf = 4096 ; 
	
			
	printf("%d\n", read_buf);
	int buff;
	int closed=0;
	int bytes=0;
	while(1){
		
	
		if(numLiveChildren < children_n){
			switch(fork()){
				case -1: 
					write(fd_file_raports, "ERROR:Error occuried while fork()\n", 35);
				 	exit(1);
					break;
				case 0:
					childDo(fd_file_raports, readfd, writefd);	
					break;
				default:
				 	numLiveChildren++;
					break;	
			}
		}
		else {
			
			/* Check if any process has termianated */
			if((returned_pid=checkStatus(fd_file_raports)) > 0)
		    	continue;

			/* Check if read failed and no process terminated */
			if(!returned_pid && !read_return ){
				nsleep(0.48); 
			}

			/* If no process exists, exit program */
			if(returned_pid == -1)
				break;


			printf("num alive=%d\n", numLiveChildren);
			printf("read-%d write-%d\n", read_bytes, written_bytes);

			/* Read data from childs and check if no error occurred */
			if((read_return=readData(readfd[0], fd_file_success,record)) == -1){
						errorExit("ERROR:Pipe is closed, cannot read");
			}
			else if(read_return == -2){
				errorExit("ERROR:Cannot write to success file");
			}	
			read_bytes += read_return;

			file_filled = read_bytes/(double)(MAX_SHORT*2);
			
			printf("file filled %lf\n", file_filled);	

			/* Do tego warunku dodaje zabezpiecznie, zeby program nie wpisywał do potoku,
			 gdy roznica jest < 55000 (nie jest to jakas specjalnie wyliczona wartosc), ale
			 maksymalna ilosc danych w potoku to 64Kb, przy buforze rzędu 2Kb, w przypadku małej ilosci
			 potomków oraz małego bloku np p=1 w=100, do pipe są wysłane co iteracje 2kB, a 
			 odbierane tylko 100 przez co potok sie zapycha, w ten sposób probuje to rozwiazac
			 poniewaz w moim rozwiazaniu nie moge ustawic writefd[1] na nieblokujacy  */
			/* Copy data from source to pipe */
			if( written_bytes < bytes_data_file && written_bytes - read_bytes < 55000){
				if( written_bytes + read_buf > bytes_data_file)
					buff = bytes_data_file - written_bytes;
				else 
					buff = read_buf;

				if((bytes = copyData(fd_file_data, writefd[1], buff)) == -1){
					errorExit("ERROR:Writing to pipe");
				}

				written_bytes += bytes;
			}
			
		
			/* Close end of pipe after copying all data */
			if(written_bytes - bytes_data_file == 0 && closed == 0){
				closed = 1;
				close(writefd[1]);

			}
			
		}

		
	}

	
	if(close(fd_file_data))
		errorExit("ERROR:Closing source file");
	
	if(close(fd_file_raports))
		errorExit("ERROR:Closing raports file");
	
	if(close(fd_file_success))
		errorExit("ERROR:Closing successes file");

	printf("Bytes recived from child processes: %d\n", read_bytes);
	printf("Bytes readed  from  source and transferred to pipe: %d\n", written_bytes);
	printf("File was filled in %0.2lf%%\n", 100*file_filled);
	

	exit(EXIT_SUCCESS);		
}