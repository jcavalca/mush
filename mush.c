# include <stdlib.h>
# include <stdio.h>
# include <ctype.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>
# include <signal.h>
# include <sys/wait.h>
# include <errno.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <sys/types.h>

# include "define.h"
# include "share.c"
# include "exec.c"
# include "pipe.c"
# include "parseline.c"



int interrupt = FALSE;

void wait_gracefully(){
       while (children--){
          if (-1 == wait(NULL)){
            perror("wait");
            exit(EXIT_FAILURE);
          }
        }

}

void exit_gracefully(){
	wait_gracefully();
        fprintf(stderr, "\n");	
	exit(EXIT_SUCCESS);
}

void read_from_stdin(){
        int current_char;
        while ( (current_char = getchar()) != EOF){
	   while (current_char == NEW_LINE || current_char == CARRIAGE_RET){
		fprintf(stderr, "8-P ");
		current_char = getchar();
	   }
	   if (current_char == EOF)
	       break;
           parseline(current_char);
	   if (interrupt == FALSE){
      	       fprintf(stderr, "8-P ");}
	   else{interrupt = TRUE;}


        }

	/* EOF */
	if (errno == NO_ERROR)
	  exit_gracefully();

	/* SIGINT */
	else if (errno == EINTR){
	  /* restore normality*/
	  errno = NO_ERROR;
	  if (children > 0)
	  wait_gracefully();
	  read_from_stdin();
	}
}

void handler(int signum) {
	if (signum == SIGINT){
	  /* catch it and continue*/
	  fprintf(stderr, "\n8-P ");
	  interrupt = TRUE; 
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
	return 0;	
} 
