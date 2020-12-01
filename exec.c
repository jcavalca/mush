/* This file has implementation for executing either 
 * cd or general commands*/

/* implemented by a parent */
void exec_cd(char *argv){
	char * token = strtok(argv, " ");
	token = strtok(NULL, " ");
	if (-1 == chdir(token)){
	     perror(token);	
	}
}


/* implemented by a child */
void exec_command2(int stage, int *argc, char *pass_argv){
        int words = 0;
        char main_argv[COMM_LEN_MAX];
	char total[COMM_LEN_MAX];
        char *array[COMM_LEN_MAX];
	char *token;
	
	strcpy(total, pass_argv);	

	token = strtok(total, " ");

	strcpy(main_argv, token);
	while (token != NULL){
            char *a = malloc(COMM_LEN_MAX);
	    strcpy(a, token);
	    array[words] = a;	
	    words++;
	    token = strtok(NULL, " ");
	}

        array[words] = NULL;
        if (-1 == execvp(main_argv, array)){
            perror(main_argv);}
}
