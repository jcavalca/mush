/* This file contains the pipelining and execing portion of mush.
 * Important to note that this implementation was built upon changing 
 * the pipeline lecture example.*/

# define WRITE_END 1
# define READ_END 0
# define MSG ""

int children = 0;

/*
void exec_command(int stage, int *argc, char *argv){
	int count, file_off = 0;
	char file[COMM_LEN_MAX];
	int words = 0;
	int len = strlen(argv);
	char main_argv[COMM_LEN_MAX];
	char *array[COMM_LEN_MAX];
	
	if (array == NULL)
	usage("malloc");
	for (count = 0; count < len; count++){
           if (argv[count] == ' '){
		char *a = calloc(COMM_LEN_MAX, 1);
		if (a == NULL)
		  usage("calloc");
		strcpy(a, file);
		array[words ] = a;
        	if(words == 0){
		strcpy(main_argv, file);
		}
	        zero_buf(file);
                file_off = 0;
		words++;
                
           }else{
        	file[file_off] = argv[count];
        	file_off++;
		if (count == len - 1){
		      char a[COMM_LEN_MAX];
        	      strcpy(a, file);
        	      array[words ] = a;
		   if (words == 0){
		        strcpy(main_argv, file);
			}
		words++;
		}
           }
	}
	array[words] = NULL;
	if (-1 == execvp(main_argv, array))
		perror(main_argv);
}
*/
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

	  if (!(child = fork())){
    	  /* child */
	    int fd_in, fd_out;
	    fd_in = STDIN_FILENO; /* default */
	    fd_out = STDOUT_FILENO;
	    if( -1 == dup2(old[READ_END], fd_in) ){
		perror("dup2 old");	
	    }

	    if( i < num - 1){
	      /* output redirection */
              if ( -1 == dup2(next[WRITE_END], fd_out)){
		perror("dup2 new");
		}
	    }
	    /* io_redirection(i, stageArr[i] -> input, stageArr[i] -> output);*/
	     /* cleaning parent's opened fd's */
	     close(old[READ_END]);
	     close(old[WRITE_END]);
	     close(next[READ_END]);		
  	     close(next[WRITE_END]);
	     exec_command(i, stageArr[i] -> argc, stageArr[i] -> argv);
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
	if (children != num)
	   return -1;
	else
	   return 0;
}


