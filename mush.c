# include <stdlib.h>
# include <stdio.h>
# include <ctype.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>
# include <signal.h>
# include <sys/wait.h>
# include <errno.h>

# include "parseline.c"


# define NO_ERROR 0


int children = 0;

void exit_gracefully(){

	while (children--){
	  if (-1 == wait(NULL)){
            perror("wait");
	    exit(EXIT_FAILURE);
	  }
	}
	
	exit(EXIT_SUCCESS);
}

void read_from_stdin(){
        int current_char;
        while ( (current_char = getchar()) != EOF){

        parseline(current_char, &children);
	
	fprintf(stderr, "8-P ");
        }

	/* EOF */
	if (errno == NO_ERROR)
	  exit_gracefully();

	/* SIGINT */
	else if (errno == EINTR){
	  /* restore normality*/
	  errno = NO_ERROR;
	  read_from_stdin();
	}
}

void handler(int signum) {
	if (signum == SIGINT){
	  /* catch it and continue*/
	  fprintf(stderr, "\n8-P ");  
	  fflush(stdout);
	}
}

int main(int argc, char *argv[]){	


	
	struct sigaction sa;
		
	 /* first set up the handler */
        sa.sa_handler = handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        if ( -1 == sigaction(SIGINT, &sa, NULL)){
          perror("sigaction");
          exit(EXIT_FAILURE);
	}
	fprintf(stderr, "8-P ");
	read_from_stdin();	
	exit(EXIT_SUCCESS);	
} 
