/* This file contains the pipelining and execing portion of mush.
 * Important to note that this implementation was built upon changing 
 * the pipeline lecture example.*/



int input_redirection(int stage, char *input){
	int ret = FALSE;
	/* Input Redirection */
	if (stage == 0 && strcmp(input, "original stdin") != 0){
	    ret = TRUE;
	}
	return ret;
}

/* test for the only two possible scenarios of output redirection */
int output_redirection(int stage, int numb_pipes, char *output){
	int ret = FALSE;
	if ( stage == 0 && numb_pipes == 0 && 
		strcmp(output, "pipe to stage 1") != 0)
              ret = TRUE;
	else if (stage == numb_pipes - 1 && strcmp(output, "original stdout") != 0)		
	      ret = TRUE;
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
	    int fd_in;
	    int test_in, test_out;
	    int redir_in = FALSE;
	    fd_in = STDIN_FILENO; /* default */

	    /* read end */
	    if (TRUE == (test_in = input_redirection(i, stageArr[i] -> input)))
	    {
		if (-1 == (test_in = open(stageArr[i] -> input, O_RDONLY))){
		    perror(stageArr[i] -> input);
		    exit(EXIT_FAILURE);
		}
	    }
	    if (test_in == FALSE){
	        if( -1 == dup2(old[READ_END], fd_in) ){
		     perror("dup2 old");	
	          }
	    }else {		
		if( -1 == dup2(test_in, STDIN_FILENO) ){
                     perror("dup2 old");
                  }
		redir_in = TRUE;
	   }

	   /* write end */	
	    if (TRUE == (test_out = output_redirection(i, numb_pipes,  stageArr[i] -> output))){
		if (-1 == (test_out = open(stageArr[i] -> output, 
			              O_WRONLY | O_CREAT | O_TRUNC, 
	   		              (S_IWUSR | S_IRUSR)))){
                    perror(stageArr[i] -> output);
                    exit(EXIT_FAILURE);
                }
		if( -1 == dup2(test_out, STDOUT_FILENO) ){
                     perror("dup2 old");
                  }
                redir_in = TRUE;
            }
	   

	    if( i < num - 1){
              if ( -1 == dup2(next[WRITE_END], STDOUT_FILENO)){
		perror("dup2 new");
		}
	    }
	     /* cleaning parent's opened fd's */
	     close(old[READ_END]);
	     close(old[WRITE_END]);
	     if (redir_in == FALSE){
	         close(next[READ_END]);		
  	         close(next[WRITE_END]);}
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
	    if (errno != EINTR)
 	    perror("wait");
	  }
	}
	if (children != num)
	   return -1;
	else
	   return 0;
}


