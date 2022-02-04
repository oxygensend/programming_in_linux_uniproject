#include "kolekcjoner.h"
#include <fcntl.h>
#include <errno.h>

#define NANOSEC 1000000000L
#define FL2NANOSEC(f) {(long)(f), ((f)-(long)(f))*NANOSEC}

void nsleep(float sec){

    struct timespec sleep_t = FL2NANOSEC(sec);

    nanosleep(&sleep_t, NULL);


}


/* 
	program kazdego potomka zastepuje nowym, ale maksymalnie moze byc ich tylko n
	jezeli potemek zakonczy status z bledem to n=n-1. Nowe potemki generujemy az plik
	zostanie wypelniony w 75% co znaczy ze zostanie wypelniony w 75%???



*/

static volatile int numLiveChildren = 0;
/* Number of children started but not yet waited on */
static void
sigchldHandler(int sig)
{
	int status;
	pid_t childPid;

	printf("handler: Caught SIGCHLD\n");
	while ((childPid = waitpid(-1, &status, WNOHANG)) > 0) {
		printf("handler: Child process %d  terminated with status - %d\n",
		childPid, WEXITSTATUS(status));
		numLiveChildren--;
	}
	if (childPid == -1 && errno != ECHILD)
		perror("error");

	nsleep(0.1);
	/* Artificially lengthen execution of handler */
	printf("handler: returning\n");
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
	int fd_file_test = open("dziala.txt", O_WRONLY);
	int fd_file_success = open(file_success, O_WRONLY | O_NONBLOCK);
	int fd_file_raports = open(file_raports, O_WRONLY | O_NONBLOCK | O_CREAT, 0664);
	
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sigchldHandler; 

	sigaction(SIGCHLD, &sa, NULL);


	struct Record record;
	int status=0;
	int pipfd[2];
	int nr;
	pipe(pipfd);
	char * fnt = "New process created! pid = %d\n";
				char tab[34];
	
	for(int i=0; i<children_n;i++){
		switch(fork()){
			case -1: perror(NULL); exit(1); break;
			case 0:
				
				snprintf(tab,sizeof(tab), fnt, getpid());
				write(fd_file_raports, &tab, sizeof(tab));
				close(STDOUT_FILENO);
				dup(pipfd[1]);
				close(STDIN_FILENO);
				dup(pipfd[0]);
				// write(pipfd[1], "32 12 24 23 123 412 53 123 213 541",100);
				copyData(fd_file_data, pipfd[1]);
				execl("poszukiwacz", "poszukiwacz",bytes_for_process, NULL);
						

				break;
			default:
						
				// read(pipfd[0], &buf, 100);
				// printf("%s\n", buf);

				break;

		}

	}
	while(1){

		nsleep(0.4);
		nr=read(pipfd[0],&record, sizeof(struct Record));
		if( nr == 0){
			printf("File closed.\n");
			break;

		}
		else {
			printf("%d\t%d\n", record.value, record.process_pid);
		}

	}

	// printf("l-%s s-%d w-%d f-%s p-%d d-%s\n", file_raports, bytes_data_file, bytes_for_process, file_success, children_n, file_data);

}