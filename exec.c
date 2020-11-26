/* this file has implementation for executing either 
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
        if (-1 == execvp(main_argv, array)){
                perror(main_argv);}
}
