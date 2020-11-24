# define COMM_LEN_MAX 512
# define PIPE_ARG_MAX 10 
# define COMM_ARG_MAX 10
# define STAGE_MAX 512
# define LARGE_SIZE 4092

typedef struct Stage{
        int *number;
        char *total;
        char *input;
        char *output;
        int *argc;
        char *argv;
} Stage;

