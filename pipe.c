/* This file contains the pipelining and execing portion of mush.
 * Important to note that this implementation was built upon changing 
 * the pipeline lecture example.*/

int children = 0;

void usage(char *name){
	fprintf(stderr, "usage:  %s <stages>\n", name);
	exit(EXIT_FAILURE);
}

# define WRITE_END 1
# define READ_END 0
# define MSG "Boo\n"

void telephone(int id){
	int c;
	printf("This is process %d (pid: %d).\n", id, getpid());
	while((EOF != (c=getchar())) )
		putchar(c);
}

int pipe_stages(Stage *stageArr[COMM_LEN_MAX], int numb_pipes){

	int num, i;
	int old[2], next[2];
	pid_t child;
	
	num = numb_pipes;
	if (pipe(old)){
		perror("pipe");
		exit(EXIT_FAILURE);
	}
	write(old[WRITE_END], MSG, strlen(MSG));

	for (i = 0; i < num; i++){
	  if (i < num - 1){
	    if (pipe(next)){	/* create new pipe */
		perror("new pipe");
		exit(EXIT_FAILURE);
	     }
	   }

	  if (!(child = fork())){
    	  /* child */
	    int fd_in, fd_out;
	    fd_in = STDIN_FILENO; /* default */

  	    /* input redirection (needs to be in the first stage) */
     	    if (children == 0 && strcmp(stageArr[children] -> input, "original stdin") != 0){
		fd_in = open(stageArr[children] -> input, O_RDONLY);
		if (fd_in == -1){
		   perror("open in");
		   exit(EXIT_FAILURE);
		}
	    }
	
	    if( -1 == dup2(old[READ_END], fd_in) ){
		perror("dup2 old");	
	    }

	    if( i < num - 1){
	      /* output redirection */
	      if (-1 == (fd_out = open(stageArr[children] -> output, O_WRONLY))){
		fd_out = STDOUT_FILENO;
	      }

              if ( -1 == dup2(next[WRITE_END], fd_out)){
		perror("dup2 new");
		}
	    }

	     /* cleaning parent's opened fd's */
	     close(old[READ_END]);
	     close(old[WRITE_END]);
	     close(next[READ_END]);		
  	     close(next[WRITE_END]);
	     telephone(i);
	     children++;
	     exit(EXIT_SUCCESS);	
	}
	  /* parent */	
	  close(old[READ_END]);
	  close(old[WRITE_END]);
	  old[READ_END] = next[READ_END];
	  old[WRITE_END] = next[WRITE_END];

	}
	
	while ( num-- ){
	  if (-1 == wait(NULL)){
	    perror("wait");
	  }
	}

	return 0;
}


