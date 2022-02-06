#include "kolekcjoner.h"
#include <fcntl.h>
#include <errno.h>



/* 
	program kazdego potomka zastepuje nowym, ale maksymalnie moze byc ich tylko n
	jezeli potemek zakonczy status z bledem to n=n-1. Nowe potemki generujemy az plik
	zostanie wypelniony w 75% co znaczy ze zostanie wypelniony w 75%???



*/

// For singal handler
static volatile int numLiveChildren = 0;
int fd_file_raports;
struct timespec tt;

/* Linux Programming Interface  chapter 26.3 example program listing 26-5 */

/* Number of children started but not yet waited on */
static void
sigchldHandler(int sig)
{
	int status;
	pid_t childPid;
    char * fnt = "Process %d terminated with status - %d at %ld.%ld\n"; 
	char tab[100];
	write(fd_file_raports,"handler: Caught SIGCHLD\n", 25);
	while ((childPid = waitpid(-1, &status, WNOHANG)) > 0) {

		clock_gettime(CLOCK_MONOTONIC, &tt);
		snprintf(tab, sizeof(tab), fnt,
		childPid, WEXITSTATUS(status), tt.tv_sec, tt.tv_nsec);
		write(fd_file_raports, tab, strlen(tab));
		numLiveChildren--; //TODO
	}
	if (childPid == -1 && errno != ECHILD)
		write(fd_file_raports, "handler: ERROR\n",16);

	nsleep(0.1);
	/* Artificially lengthen execution of handler */
	write(fd_file_raports,"handler: returning\n", 20);
}
/* ------------------------------------------------------------------- */

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
	int fd_file_test = open("dziala.txt", O_WRONLY);
	int fd_file_success_r = open(file_success, O_RDONLY | O_NONBLOCK);
	int fd_file_success_w = open(file_success, O_WRONLY | O_NONBLOCK);
    fd_file_raports = open(file_raports, O_WRONLY | O_NONBLOCK | O_CREAT, 0664);
	
   /* filling success file with null */
	pid_t p = 0;
	for(int i=0;i<65536;i++){
		write(fd_file_success_w,&p,sizeof(pid_t));
	}


	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sigchldHandler; 
	sigaction(SIGCHLD, &sa, NULL);


	struct Record record;
	int status=0;
	int readfd[2], writefd[2];
	int nr;
	pipe(readfd);
	pipe(writefd);

	char * fnt = "New process created pid = %d at %ld.%ld \n";
	char tab[100];

	/* Linux Programming Interface chapter 44.9 */
	int flags = fcntl(readfd[0], F_GETFD);
	flags |= O_NONBLOCK;
	fcntl(readfd[0], F_SETFL, flags);
	/* ---------------------------------------- */


	 
	for(int i=0; i<children_n;i++){

		copyData(fd_file_data, writefd[1]);

		switch(fork()){
			case -1: perror(NULL); exit(1); break;
			case 0:
				clock_gettime(CLOCK_MONOTONIC, &tt);
				snprintf(tab,sizeof(tab), fnt, getpid(), tt.tv_sec, tt.tv_nsec);
				write(fd_file_raports, &tab, strlen(tab));

				dup2(readfd[1], STDOUT_FILENO);
				close(writefd[1]);
				close(readfd[1]);
				dup2(writefd[0], STDIN_FILENO);
				close(readfd[0]);
				close(writefd[0]);

				execl("poszukiwacz", "poszukiwacz",bytes_for_process, NULL);

				break;
			default:
				break;	
		}

	}

	
	nsleep(1);

	close(writefd[0]);
	close(readfd[1]);

		/* Parent reads data */
	while(1){
	    nr=read(readfd[0],&record, sizeof(struct Record));
		if(nr == 0){
			break;
		}
		else if(nr == -1 && errno==EAGAIN){
			break;
		}
		else{
			printf("%hd\t%d\n", record.value, record.process_pid);
			writeSuccess(record.value*sizeof(pid_t), 
						fd_file_success_r, 
						fd_file_success_w,
						record.process_pid
						);
		}
	}

	

}