#include "kolekcjoner.h"

int main(int argc, char ** argv){


    char c;
    while (( c = getopt(argc, argv, "d:s:w:f:l:p:")) != -1){
	
		if( optarg && optarg[0] == '-' && optarg[1] ){
			optopt = c + '\0';
			c = 63;
			optind--;
			}

		if(readFlags(c))
			return 1;
	}

    if(checkFlags()){
        return 11;
    }

}