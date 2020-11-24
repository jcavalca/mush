# include <stdlib.h>
# include <stdio.h>
# include <string.h>


# define COMM_LEN_MAX 512
# define PIPE_ARG_MAX 10 
# define COMM_ARG_MAX 10
# define STAGE_MAX 512
# define LARGE_SIZE 4092

void zero_buf(char buf[COMM_LEN_MAX]){
        int count;
        for (count = 0; count < COMM_LEN_MAX; count++)
        buf[count] = 0;
}

void print_stage(int stage, char *total, 
	char *input, char *output, int argc, char *argv){
	int count;
	char file[COMM_LEN_MAX];
	int file_off = 0;
	zero_buf(file);
	printf("\n");	
	printf("--------\n");
	printf("Stage %d: \"%s\"\n", stage, total);
	printf("--------\n");
	printf("%10s: %s\n", "input", input);
	printf("%10s: %s\n", "output", output);
	printf("%10s: %d\n", "argc", argc);
	printf("%10s: ", "argv");
	/*When argc > 1*/
	for (count = 0; count < strlen(argv); count++){
	if (argv[count] == ' '){
		if (count != strlen(argv) - 1)
		printf("\"%s\",", file);
		else
		printf("\"%s\"\n", file);
		zero_buf(file);
		file_off = 0;
		}
	else{
	file[file_off] = argv[count];
	file_off++;
	if (count == strlen(argv) - 1)
	printf("\"%s\"\n", file);}
	}
}


int parseline(int read_char, int *children_p){
	int count;
	char arg[COMM_LEN_MAX];
	int sum = 0;
	int numb_pipes = 1; /* #pipes = #| + 1*/
	char current = '\0';
	char temp[2];
	zero_buf(arg);

	/* read first char when checking on mush */
	temp[0] = (char) read_char;
        temp[1] = '\0';	
	strcat(arg, temp);
	
	while ( (current = fgetc(stdin)) != '\n'){
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
	
	/*Parsing ...*/
	/*For each stage do*/
	for (count = 0; count < numb_pipes; count++){
	char *token, *gen_input, *gen_output,
	 pass_argv[COMM_LEN_MAX], total[COMM_LEN_MAX], temp[COMM_LEN_MAX],
	 pipe_input[COMM_LEN_MAX], pipe_output[COMM_LEN_MAX],
	input[COMM_LEN_MAX], output[COMM_LEN_MAX];
	int pass_argc = 1, elements = 1;
	if (count == 0)
      	token = strtok(arg, " ");
	else
	token = strtok(NULL, " ");
		/*Empty pipe*/	
	if (token == NULL || strcmp(token, "|") == 0){
	fprintf(stderr, "invalid null command\n");
	exit(EXIT_FAILURE);
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
			print_stage(count, total, input, output, 
				pass_argc, pass_argv);			
			break;	}

		/*Input redirection*/
		else if (strcmp(token, "<") == 0){
		elements++;
			/*More than one input*/
		if (strcmp(input, gen_input) != 0 && 
			strcmp(input, pipe_input) != 0){
		fprintf(stderr, "cmd: bad input redirection\n");
		exit(EXIT_FAILURE);
		}
		zero_buf(input);
		strcpy(input, strtok(NULL, " "));
			/*Invalid input*/
		if (input == NULL || strcmp(input, "<") == 0 ||
		strcmp(input, ">") == 0 || strcmp(input, "|") == 0){
		fprintf(stderr, "cmd: bad input redirection\n");
                exit(EXIT_FAILURE);
		}

			/*Both input redirection and a pipe in*/
		if (count > 0){
		fprintf(stderr, "cmd: ambigous input\n");
                exit(EXIT_FAILURE);
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
                exit(EXIT_FAILURE);
                }
		zero_buf(output);
                strcpy(output, strtok(NULL, " "));
               		/*Invalid output*/
                if (output == NULL || strcmp(output, "<") == 0 ||
                strcmp(output, ">") == 0 || strcmp(output, "|") == 0){
                fprintf(stderr, "cmd: bad output redirection\n");
                exit(EXIT_FAILURE);
                }

                	/*Both output redirection and a pipe out*/
                if (count == numb_pipes - 2){
                fprintf(stderr, "cmd: ambigous output\n");
                exit(EXIT_FAILURE);
                }
		strcat(total, " > ");
                strcat(total, output);
		}

		/*New pipe*/
		else if ( strcmp(token, "|") == 0){
		 print_stage(count, total, input, output,
                                pass_argc, pass_argv);
                        break; 
		}
						
		/*Extra arguments*/		
		else {
		pass_argc++;
		elements++;
		if (pass_argc >= COMM_ARG_MAX){
		fprintf(stderr, "cmd: too many arguments\n");
		exit(EXIT_FAILURE);	
		}
		strcat(pass_argv, " ");
		strcat(pass_argv, token);
		strcat(total, " ");
		strcat(total, token);
		}
		if (elements >= PIPE_ARG_MAX){
		fprintf(stderr, "pipeline too deep\n");
                exit(EXIT_FAILURE);
		}
		}
		
        }

	return 0;
}


/*Big thanks to strtok writer ...*/
