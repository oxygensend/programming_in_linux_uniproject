#include "kolekcjoner.h"


short getShort(char *str){

    char * endptr;
    short x = (short)strtol(str, &endptr,10);
    if(*endptr){
        perror("Bad number format, gives 2 bytes number");
        return -1;
    }
    return x;

}

int getBytes(char *str){

    short x;
    if(str[strlen(str)-1] == 'M'){
        char subbuff[strlen(str)-1];
        memcpy( subbuff, str, strlen(str)-1 );
        x = getShort(subbuff) * 1024 * 1024;
    }
    else if( str[strlen(str)-1] == 'K'){
        char subbuff[strlen(str)-1];
        memcpy( subbuff, str, strlen(str)-1 );
        x = getShort(subbuff)  * 1024;
    }
    else {
        x = getShort(str);
    }
        
    return x;

}

int readFlags(char c){
		
		char * endptr;
		switch(c) {
		
			case 'd':
				flag_d = 1;
				break;
			case 's':
				flag_s = 1;
				break;
			case 'w':
				flag_w = 1;
				break;
 			case 'f':
                flag_f = 1;
				break;
 			case 'l':
                flag_l = 1;
 				break;
 			case 'p':
                flag_p = 1;
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



