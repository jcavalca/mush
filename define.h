# define COMM_LEN_MAX 512
# define PIPE_ARG_MAX 10 
# define COMM_ARG_MAX 10
# define STAGE_MAX 512
# define LARGE_SIZE 4092
# define TRUE 1
# define FALSE 0
# define WRITE_END 1
# define READ_END 0
# define NO_ERROR 0
# define NEW_LINE 10
# define CARRIAGE_RET 13



typedef struct Stage{
        int *number;
        char *total;
        char *input;
        char *output;
        int *argc;
        char *argv;
} Stage;

