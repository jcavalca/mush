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
int file_in = FALSE, file_out = FALSE;

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
        while ( (current_char = fgetc(mush_input)) != EOF){
	   while (current_char == NEW_LINE || current_char == CARRIAGE_RET){
		if (file_in == FALSE && file_out == FALSE)
		fprintf(stderr, "8-P ");
		current_char = fgetc(mush_input);
	   }
	   if (current_char == EOF)
	       break;
           parseline(current_char);
	   if (interrupt == FALSE){
      	      if (file_in == FALSE && file_out == FALSE)
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
	  if (file_in == FALSE && file_out == FALSE)
	  fprintf(stderr, "\n8-P ");
	  else{
	  fprintf(stderr, "\n");
	  }
	  interrupt = TRUE; 
	}
}

void command_parser(int argc, char *argv[], int *in_ptr, int *out_ptr){ 
	if (!isatty(STDIN_FILENO))
           *in_ptr = TRUE;

        if (!isatty(STDOUT_FILENO))
            *out_ptr = TRUE;

	if (argc == 2){
	    if (!*in_ptr){      
	        *in_ptr = TRUE;
		if (NULL == (mush_input = fopen(argv[1], "r"))){
		     fprintf(stderr, "could not open %s\n", argv[1]);
		     exit(EXIT_FAILURE);
		}
	    }
	}
	else{
		if ( argc > 2)
		fprintf(stderr, "Usage: mush (input file)\n");
	}



}

int main(int argc, char *argv[]){	

	struct sigaction sa;
	 /* set up handler for ^C */
        sa.sa_handler = handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        if ( -1 == sigaction(SIGINT, &sa, NULL)){
          perror("sigaction");
          exit(EXIT_FAILURE);
	}
	mush_input = stdin; /* default */
	command_parser(argc, argv, &file_in, &file_out);
	if (file_in == FALSE && file_out == FALSE)
	fprintf(stderr, "8-P ");
	read_from_stdin();	
	return 0;	
} 
