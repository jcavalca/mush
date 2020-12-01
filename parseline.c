/* This file contains implementation for parsing command line
 * so that we can evetually use properly pipes and execs */

void add_stage(Stage* array[COMM_LEN_MAX], int stage, char *total, 
        char *input, char *output, int argc, char *argv){

	Stage *new = malloc(sizeof(Stage));
	int *new_int = malloc(sizeof(int));
	int *new_argc = malloc(sizeof(int));

	char *new_total = malloc(strlen(total) + 1);
        char *new_input = malloc(strlen(input) + 1);
        char *new_output = malloc(strlen(output) + 1);
        char *new_argv = malloc(strlen(argv) + 1);	
	


	/* error checking */	
	if (new == NULL || new_int == NULL || new_total == NULL ||
	    new_input == NULL || new_output == NULL || 
	    new_argc == NULL || new_argv == NULL){
	perror("malloc");
	exit(EXIT_FAILURE);
	}

	/* assigning to Stage*/
	*new_int = stage;
	new -> number = new_int;
	
	strcpy(new_total, total);
	new -> total = new_total;

	strcpy(new_input, input);
	new -> input = new_input;

	strcpy(new_output, output);
	new -> output = new_output;
	
	*new_argc = argc;
	new -> argc = new_argc;	

	strcpy(new_argv, argv);
	new -> argv = new_argv;

	array[stage] = new;

}


int parseline(int read_char){
	int count;
	char arg[COMM_LEN_MAX];
	int sum = 0;
	int numb_pipes = 1; /* #pipes = #| + 1*/
	char current = '\0';
	char temp[2];
	char file[COMM_LEN_MAX];
	Stage *stageArr[COMM_LEN_MAX];
	zero_buf(arg);
	zero_buf2(stageArr);

	/* read first char when checking on mush */
	temp[0] = (char) read_char;
        temp[1] = '\0';	
	strcat(arg, temp);
	
	while ( (current = fgetc(mush_input)) != '\n'){
	  sum++;	
	  if (sum > COMM_LEN_MAX){
	    fprintf(stderr, "command too deep");
  	    exit(EXIT_FAILURE);
	  }
	  temp[0] = current;
	  temp[1] = '\0';
	  strcat(arg, temp);
	}

	/*How many pipes do we have ...*/
	for (count = 0; count < COMM_LEN_MAX; count++){
	if (arg[count] == '|')
	numb_pipes++;
	}	
	if (numb_pipes > PIPE_ARG_MAX){
	fprintf(stderr, "pipeline too deep\n");
 	return 0;
	} 	
	/*Parsing ...*/
	/*For each stage do*/
	for (count = 0; count < numb_pipes; count++){
	char *token, *gen_input, *gen_output,
	 pass_argv[COMM_LEN_MAX], total[COMM_LEN_MAX], temp[COMM_LEN_MAX],
	 pipe_input[COMM_LEN_MAX], pipe_output[COMM_LEN_MAX],
	input[COMM_LEN_MAX], output[COMM_LEN_MAX];
	int pass_argc = 1, elements = 1;
	if (count == 0){
      	token = strtok(arg, " ");
	strcpy(file, token);
	}
	else
	token = strtok(NULL, " ");
		/*Empty pipe*/	
	if (token == NULL || strcmp(token, "|") == 0){
	fprintf(stderr, "invalid null command\n");
	return 0;
	}

		/*Input and Output*/
	gen_input = "original stdin";
        gen_output = "original stdout";
	
	zero_buf(temp);
	zero_buf(pipe_input);
	sprintf(temp, "%d", count - 1);
	strcat(pipe_input, "pipe from stage ");
	strcat(pipe_input, temp);
	
       	zero_buf(temp);
	zero_buf(pipe_output);
	sprintf(temp, "%d", count + 1);
        strcat( pipe_output, "pipe to stage ");
	strcat( pipe_output, temp);
	
	zero_buf(input);
	zero_buf(output);
	if (count == 0){
	strcpy(input, gen_input);
	if (count + 1 == numb_pipes)
	strcpy(output, gen_output);
	else
	strcpy(output, pipe_output);
	}else if (count == numb_pipes - 1){
	strcpy(input, pipe_input);
	strcpy(output, gen_output);
	}else{
	strcpy(input, pipe_input);
	strcpy(output, pipe_output);	
	}
	

	zero_buf(pass_argv);
	zero_buf(total);
	/*Getting argv*/
	strcpy(pass_argv, token);
	strcpy(total, pass_argv);
	/*Breaks when changing pipes or at end of command line*/
	for (;;){
		token = strtok(NULL, " ");
		/*End of command-line*/
		if ( token == NULL){
			/*print_stage(count, total, input, output, 
				pass_argc, pass_argv);	*/		
			add_stage(stageArr, count, total, input,
				 output, pass_argc, pass_argv);
			break;	}

		/*Input redirection*/
		else if (strcmp(token, "<") == 0){
		elements++;
			/*More than one input*/
		if (strcmp(input, gen_input) != 0 && 
			strcmp(input, pipe_input) != 0){
		fprintf(stderr, "cmd: bad input redirection\n");
		return 0;
		}
		zero_buf(input);
		strcpy(input, strtok(NULL, " "));
			/*Invalid input*/
		if (input == NULL || strcmp(input, "<") == 0 ||
		strcmp(input, ">") == 0 || strcmp(input, "|") == 0){
		fprintf(stderr, "cmd: bad input redirection\n");
                return 0;
		}

			/*Both input redirection and a pipe in*/
		if (count > 0){
		fprintf(stderr, "cmd: ambigous input\n");
                return 0;
		}
		strcat(total, " < ");
		strcat(total, input);
		}

		/*Output redirection*/
		else if (strcmp(token, ">") == 0){
		elements++;
			/*More than one output*/
                if (strcmp(output, gen_output) != 0 && 
			strcmp(output, pipe_output) != 0){
                fprintf(stderr, "cmd: bad output redirection\n");
                return 0;
                }
		zero_buf(output);
                strcpy(output, strtok(NULL, " "));
               		/*Invalid output*/
                if (output == NULL || strcmp(output, "<") == 0 ||
                strcmp(output, ">") == 0 || strcmp(output, "|") == 0){
                fprintf(stderr, "cmd: bad output redirection\n");
                return 0;
                }

                	/*Both output redirection and a pipe out*/
                if (count == numb_pipes - 2){
                fprintf(stderr, "cmd: ambigous output\n");
                return 0;
                }
		strcat(total, " > ");
                strcat(total, output);
		}

		/*New pipe*/
		else if ( strcmp(token, "|") == 0){
		/* print_stage(count, total, input, output,
                                pass_argc, pass_argv);*/
        	 add_stage(stageArr, count, total, input, 
                                 output, pass_argc, pass_argv); 
	               break; 
		}
						
		/*Extra arguments*/		
		else {
		pass_argc++;
		elements++;
		if (pass_argc - 1> COMM_ARG_MAX){ /* excludes file */
		fprintf(stderr, "cmd: too many arguments\n");
		return 0;	
		}
		strcat(pass_argv, " ");
		strcat(pass_argv, token);
		strcat(total, " ");
		strcat(total, token);
		}
		}
		
        }
	if (strcmp(file, "cd") == 0){
		exec_cd(stageArr[0] -> argv);
		return 0; 
	}
	
	if (pipe_stages(stageArr, numb_pipes) != -1)
	return numb_pipes;
	else
	return -1;
}


/*Big thanks to strtok writer ...*/
