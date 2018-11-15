#ifndef MYSH_HEADER
#define MYSH_HEADER


/* [> Libraries <] */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <stdint.h>


/* [> Defines <] */
#define PARAMS_BUFSIZE 	21
#define INPUT_BUFSIZE 	120
#define PATH_BUFSIZE 	1024
#define NO_BUILTINS 	5
#define MAX_BLOCKS 		64
#define BLOCK_SIZE 		8
#define ARGS_DELIM 		" \t\r\n\a\f\v\b\0"
#define TRUE 			1
#define FALSE 			0
#define BG_SIGN 		"&"


/* [> Structs <] */

/*
 * Struct:  md
 * --------------------
 * 	Metadata struct for storing commands in history. Used as a node/element i a linked list.
 *
 * 	*next: Pointer to next md in list 
 * 	len: Length of command 
 * 	d_index: Array for storing index pointers to datablocks in history memory datastructure.
 *
 */
typedef struct md{
    struct md *next;
	int len;
	int d_index[15];
}md;

/*
 * Struct:  h_mem
 * --------------------
 * 	Struct for allocating memory to store history in.
 *
 * 	bm: Bitmap location
 * 	hist: Data blocks location
 *
 */
typedef struct h_mem{
	unsigned char bm[BLOCK_SIZE];
	char hist[MAX_BLOCKS*BLOCK_SIZE];
} h_mem;

/*
 * Struct:  job
 * --------------------
 * 	Struct for storing job/process information.
 *
 * 	pid: Process pid of the job
 * 	cmd: Command run to start the job
 *
 */
typedef struct job{
	pid_t pid;
	char cmd[INPUT_BUFSIZE];
} job;

/*
 * Struct:  mysh
 * --------------------
 * 	Struct for storing information and memory-locations for the shell
 *
 * 	signal_flag: Signal flag for the signal handler.
 * 	cur_user: Current username.
 * 	head: Pointer to the head of the md linked list.
 * 	jobs: Pointer to the dynamically struct array for storing jobs.
 * 	no_jobs: Number of jobs running.
 *
 */
typedef struct mysh{
	volatile sig_atomic_t signal_flag;
    char cur_user[INPUT_BUFSIZE];
	struct md *head;
	struct job *jobs;
	size_t no_jobs;
} mysh;


/* [> Main mysh functions (../src/mysh.c) <] */
void sighandler(int);

void loop();

void init();

int read_stdin(char *str);

int strtok_param(char *str, char **saveptr);

int param_parser(char **param, int no_params);

int exec_process(char **args);

int cmds_len();

int exec_command(char *cmd, char *argv[]);

void debug_bitmap(unsigned char *a);

void debug_datablocks(char *a);

int save_command(char *line);

int save_job(pid_t pid, char **cmd);

int remove_job(pid_t pid);


/* [> Built in functions (../src/bi.c) <] */
int mysh_quit(char **args);

int mysh_type(char **args);

int mysh_h(char **args);

int mysh_jobs(char **args);

int mysh_kill(char **args);


/* [> Functions for history metadata structure (../src/mdll.c)<] */
void push(md **head, int len, int *d_i);

md *pop(md *head);

void remove_all(md *head);

md *remove_n(md *head, int n);


/* [> Functions for history bitmap (../src/bm.c) <] */
int test_n_free_bit(unsigned char *bitmap, int n);

int get_free_bit(unsigned char *bitmap);

void set_bit(unsigned char *bitmap, int i);

void free_bit(unsigned char *bitmap, int i);
#endif
