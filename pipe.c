/* This file contains the pipelining and execing portion of mush.
 * Important to note that this implementation was built upon changing 
 * the pipeline lecture example.*/

# define WRITE_END 1
# define READ_END 0


int input_redirection(int stage, char *input){
	int ret = -1;
	/* Input Redirection */
	if (stage == 0 && strcmp(input, "original stdin") != 0){
	    ret = 1;
	}

	return ret;
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
	for (i = 0; i < num; i++){
	  if (i < num - 1){
	    if (pipe(next)){	/* create new pipe */
		perror("new pipe");
		exit(EXIT_FAILURE);
	     }
	   }
          children++;
	  if (!(child = fork())){
    	  /* child */
	    int fd_in, fd_out;
	    int test_in;
	    fd_in = STDIN_FILENO; /* default */
	    fd_out = STDOUT_FILENO;
	    if (-1 != (test_in = input_redirection(i, stageArr[i] -> input)))
	    {
		if (-1 == (test_in = open(stageArr[i] -> input, O_RDONLY))){
		    perror(stageArr[i] -> input);
		    exit(EXIT_FAILURE);
		}
	/*	dup2(test_in, fd_in);*/	
	    }
	    if (test_in == -1){
	        if( -1 == dup2(old[READ_END], fd_in) ){
		     perror("dup2 old");	
	          }
	    }else {		
		if( -1 == dup2(old[READ_END], test_in) ){
                     perror("dup2 old");
                  }
	   }
	    if( i < num - 1){
	      /* output redirection */
              if ( -1 == dup2(next[WRITE_END], fd_out)){
		perror("dup2 new");
		}
	    }
	     /* cleaning parent's opened fd's */
	     close(old[READ_END]);
	     close(old[WRITE_END]);
	     close(next[READ_END]);		
  	     close(next[WRITE_END]);
	     exec_command(i, stageArr[i] -> argc, stageArr[i] -> argv);
	     exit(EXIT_FAILURE); /* exec failed */
	}
	  /* parent */	
	  close(old[READ_END]);
	  close(old[WRITE_END]);
	  old[READ_END] = next[READ_END];
	  old[WRITE_END] = next[WRITE_END];

	}
	
	while ( num-- ){
	  children--;
	  if (-1 == wait(NULL)){
	    perror("wait");
	  }
	}
	if (children != num)
	   return -1;
	else
	   return 0;
}


