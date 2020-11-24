# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

int children = 0;

void usage(char *name){
	fprintf(stderr, "usage:  %s <stages>\n", name);
	exit(EXIT_FAILURE);
}

# define WRITE_END 1
# define READ_END 0
# define MSG "Boo\n"

void telephone(int id){

	printf("This is process %d (pid: %d).\n", id, getpid());
	while((EOF != (c=getchar())) )
		putchar(c);
}

int pipe_stages(Stage *stageArr[COMM_LEN_MAX], int numb_pipes){

	int num, i;
	char *end;
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
	    children++;
	    if( -1 == dup2(old[READ_END], STDIN_FILENO) ){
		perror("dup2 old");	
	    }
	    if( i < num - 1){
              if ( -1 == dup2(next[ WRITE_END], STDOUT_FILENO)){
		perror("dup2 new");
		}
	    }

	     /* cleaning parent's opened fd's */
	     close(old[READ_END]);
	     close(old[WRITE_END]);
	     close(next[READ_END]);		
  	     close(next[WRITE_END]);
	     telephone(i);
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


