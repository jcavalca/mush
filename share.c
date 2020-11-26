# define NO_ERROR 0
# define FALSE 0
# define TRUE 1


int children = 0;


void usage(char *name){
	perror(name);
	exit(EXIT_FAILURE);

}

void zero_buf(char buf[COMM_LEN_MAX]){
        int count;
        for (count = 0; count < COMM_LEN_MAX; count++)
        buf[count] = 0;
}

void zero_buf2(Stage *buf[COMM_LEN_MAX]){
        int count;
        for (count = 0; count < COMM_LEN_MAX; count++)
        buf[count] = 0;
}
