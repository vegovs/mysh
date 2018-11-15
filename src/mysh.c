#include "mysh.h"


/* History memory */
struct h_mem h_m;
/* Mysh info struct */
struct mysh *m;


/* Commands */
char *builtins_cmds[] = {
    "quit",
    "type",
    "h",
    "jobs",
    "kill"
};


/* Built in function pointers*/
static int (*builtins[]) (char **) = {
    &mysh_quit,
	&mysh_type,
	&mysh_h,
	&mysh_jobs,
	&mysh_kill
};


/* Signal handler */
void sighandler(int sig){
	printf("\nCaught signal %d, exiting mysh..\n", sig);
	m->signal_flag = TRUE;
	}


/*
 * Function:  main 
 * --------------------
 *  Initializes needed memory and datatypes, runs the main loop and cleans up memory before exit.
 */
int main() {

	/* [> Start shell.. <] */
	init();
	loop();

	/* [> CLEANUP <] */
	/* Remove, free and kill all jobs */
	for(int i = 0; i < m->no_jobs; i++){
		kill(m->jobs[i].pid, SIGKILL);
	}
	/* Cleanup history */
	remove_all(m->head);
	/* Free jobs array */
	free(m->jobs);
	m->jobs = NULL;
	/* Free shell struct */
	free(m);
	m = NULL;

	return EXIT_SUCCESS;
}

/*
 * Function:  init
 * --------------------
 * 	Initializes information struct for the shell, signal handler and jobs array.
 *
 */
void init(){

	/* Initialize mysh info struct */
	m = (struct mysh*) malloc(sizeof(struct mysh));
	m->signal_flag = FALSE;
	strcpy(m->cur_user, getenv("USER"));
	m->head = NULL;
	m->jobs = (job*)malloc(sizeof(job));

	if(m->jobs == NULL) {
		fprintf(stderr, "ERROR(init): Failed to allocate memory\n");
		exit(EXIT_FAILURE);
	}

	m->no_jobs = 0;

	/* Initialize signal handler */
	if(signal(SIGINT, &sighandler) == SIG_ERR){
		fprintf(stderr, "ERROR: Could not set signal handler\n");
		exit(EXIT_FAILURE);
	}
}


/*
 * Function:  loop
 * --------------------
 *  Main loop of the shell that does the following: 
 *  1: Delete non-existing jobs/processes from the jobs-array. (using kill and remove_job)
 *  2: Print prompt 
 *  3: Read input (using read_stdin)
 *  4: Save the command (using save_command)
 *  5: Split input to tokens (using strtok_param)
 *  6: Parse tokens and execute command (using param_parser)
 */
void loop() {

	/* Input line buffer*/
	char input[INPUT_BUFSIZE];
	/* Split input line to parameters */
	char *param[PARAMS_BUFSIZE];
	/* Command counter for prompt */
	int prompt_counter = 0;

	/* Main loop */
	while(TRUE){

		/* Remove non-existing jobs from array */
		for(int i = 0; i < m->no_jobs; i++){
			if(kill(m->jobs[i].pid, 0) == -1){
				remove_job(m->jobs[i].pid);
				m->no_jobs--;
				m->jobs = (job*)realloc(m->jobs, (m->no_jobs) * sizeof(job));
			}
		}

		/* Print prompt */
		printf("%s@mysh %d> ", getenv("USER"), prompt_counter);

		/* Read input */
		if(!read_stdin(input)){
			break;
		}

		/* Save command */
		if(save_command(input) == -1){
			/* Remove last element in history */
			do{
				md *delete_me = pop(m->head);
				/* Clear bits and free memory */
				for(int j = 0; delete_me->d_index[j] >= 0; j++){
					free_bit(h_m.bm, delete_me->d_index[j]);
					int hist_index = delete_me->d_index[j]*BLOCK_SIZE;
					memset(&h_m.hist[hist_index], '\0', BLOCK_SIZE);
				}
				free(delete_me);
			}while(save_command(input) == -1);
		}

		int no_tokens;

		/* Split input to tokens */
		no_tokens = strtok_param(input, param);

		/* Increase command counter */
		if(param[0]){
			prompt_counter++;
		}

		/* Parse tokens and run commands */
		if(param[0]){
			if(param_parser(param, no_tokens) == -1){
				break;
			}
		}
	}
}


/*
 * Function:  read_stdin
 * --------------------
 *  Reads a line from stdin and stores in in *str.
 *
 *  *lineptr: Pointer to allocated memory for the line.
 *
 *  returns: 1 on succsess, 0 on error, SIGNAL or CTRL-D.
 */
int read_stdin(char *lineptr){

	/* Read input line and exit of CTRL-D is caught */
	if(fgets(lineptr, sizeof(char) * INPUT_BUFSIZE, stdin) == NULL){
		printf("\nCTRL-D caught, exiting mysh..\n");
		return 0;
	}

	/* Signal */
	if(m->signal_flag){
		return 0;
	}

#ifdef DEBUG
	fprintf(stderr, "DEBUG: Read: %s", lineptr);
#endif
	return 1;
}


/*
 * Function: strtok_param
 * --------------------
 *  Breaks a string into sequences of tokens, i.e splits a line into words. Last index saved to saveptr is NULL.
 *
 *  *str: Line to be split
 *  **saveptr: Pointer to where the tokens will be saved.
 *
 *  returns: Number of tokens, 0 if no tokens.
 */
int strtok_param(char *str, char **saveptr){

	char *token;
	int pos = 0;

	/* Save word-pointers to saveptr using strtok */
	token = strtok(str, ARGS_DELIM);

	while(token != NULL){
		saveptr[pos++] = token;
		token = strtok(NULL, ARGS_DELIM);
	}

	saveptr[pos] = NULL;

#ifdef DEBUG
	fprintf(stderr, "DEBUG: Number of tokens saved to saveptr: %d\n", pos);
	for(int i = 0; saveptr[i]; i++){
		fprintf(stderr, "DEBUG: [%d]: %s\n", i, saveptr[i]);
	}
#endif

	return pos;
}


/*
 * Function: param_parser
 * --------------------
 *  Interperets the input line and performs the command in a child using fork.
 *
 *  param: Array with parameters in each index
 *  no_params: Number of parameters in param
 *
 *  returns: 0 succsess or nothing done, 1 if quit and -1 on fork error.
 */
int param_parser(char **param, int no_params){

	/* Do nothing when params empty*/
	if(param[0] == NULL){
		return 0;
	}

	/* Run if builtin */
	for(int i = 0; i < NO_BUILTINS; i++){
	    if(strcmp(param[0], builtins_cmds[i]) == 0){
			return builtins[i](param);
		}
	}

	pid_t pid_cmd;
	pid_cmd = fork();

	/* Child */
	if(pid_cmd == 0){
		/* Background flag */
		if(strcmp(param[no_params - 1], BG_SIGN) == 0){
			param[no_params - 1] = NULL;
			setpgid(pid_cmd, 0);
			if(!exec_command(param[0], param)){
				exit(EXIT_SUCCESS);
			}
			exit(EXIT_FAILURE);
		}
		/* No background flag */
		if(!exec_command(param[0], param)){
			exit(EXIT_SUCCESS);
		}
		exit(EXIT_FAILURE);
	}
	/* Fork failed */
	else if(pid_cmd < 0){
		fprintf(stderr, "ERROR: Unable to fork");
		return -1;
	}
	/* Parent */
	else{
		if(strcmp(param[no_params - 1], BG_SIGN) == 0){
			save_job(pid_cmd, param);
			printf("[%ld] %d\n", m->no_jobs, pid_cmd);
			return 0;
		}
		wait(NULL);
		return 0;
	}
}


/*
 * Function: exec_command
 * --------------------
 *  Searches "PATH" variable for executable and runs execve on cmd.
 *
 *  *cmd: Executable/command to run
 *  *argv[]: Executable arguments
 *
 *  returns: 0 on succsessful execve, 1 if executable does not exists.
 */
int exec_command(char *cmd, char *argv[]){

	char *path = getenv("PATH");
	char *dir;
	char *envp[] = { NULL };
	char filename[PATH_BUFSIZE];

	/* Find executable */
	for(dir = strtok(path, ":" ); dir; dir = strtok(NULL,":")){
		sprintf(filename, "%s/%s", dir, cmd);
		/* Run executable if fond */
		if(access(filename,F_OK) != -1 ){
			execve(filename, argv , envp);
			return 0;
		}
	}

	printf("mysh: %s: command not found\n", cmd);
	return 1;
}


void debug_bitmap(unsigned char *a){

	printf("\nDEBUG - BITMAP\n\n");
	for(int i = 0; i < 8; i++){
		if(i == 4){
			printf("\n");
		}
		for(int j = 0; j < 8; j++){
		  printf("%d", !!((a[i] << j) & 0x80)); 	
		}
	}
	printf("\n\n");
}

void debug_datablocks(char *a){
	
	printf("DEBUG - DATABLOCKS\n\n");
	for(int i = 0; i < 64; i++){
			if((i != 0) &&  (i % 4 == 0)){
						printf("##");
						printf("\n");
					}
			printf("##%-8.8s", a + (i*8));
		}
	printf("##");
	printf("\n\n");
}


/*
 * Function: save_command
 * --------------------
 *  Save the command to history. Allocates needed blocks and sets bits in bitmap.
 *
 *  *line: Command line to save.
 *
 *  returns: 0 on success, 1 if line was empty, -1 when not enough available blocks.
 */
int save_command(char *line){

	/* Number of blocks to allocate the line in */
	int num_blocks = 0;
	int d_i[15];
	memset(d_i, -1, sizeof(d_i));
	size_t line_len = strlen(line) -1;
	int l = 0;

	/* Return if empty line */
	if(!line_len){
		return 1;
	}

	/* Calculate number of blocks to use */
	while(line_len > num_blocks*8){
		num_blocks++;
	}

	/* Test for block allocation */
	if(!test_n_free_bit(h_m.bm, num_blocks)){
		return -1;
	}

	/* Save command */
	for(int i = 0; i < num_blocks; i++){
		/* Get block indexes */
		d_i[i] = get_free_bit(h_m.bm);
		/* Set bit */
		set_bit(h_m.bm, d_i[i]);
		/* Memory byte offset */
		int x;
		/* Allocate line to blocks */
		for(x = 0; x < 8; x++){
			if(line[l] == '\n'){
				h_m.hist[d_i[i]*8 + x] = '\0';
				break;
			}
			h_m.hist[d_i[i]*8 + x] = line[l];
			l++;
		}
	}

#ifdef DEBUG
	debug_bitmap(h_m.bm);
	debug_datablocks(h_m.hist);
#endif

	push(&m->head, line_len, d_i);
	return 0;
}


/*
 * Function: save_job
 * --------------------
 *  Saves a job to the job array.
 *
 *  pid: Pid of the process.
 *  **cmd: Command entered.
 *
 *  returns: 0 on succsess, 1 if job not found.
 */
int save_job(pid_t pid, char **cmd){

	job new_job;

	new_job.pid = pid;
	new_job.cmd[0] = '\0';

	/* Save command entered, exclude '&' */
	for(int i = 0; cmd[i] && (strcmp(cmd[i], BG_SIGN) != 0); i++){
		strcat(new_job.cmd, cmd[i]);
		/* Fix missing space */
		strcat(new_job.cmd, " ");
	}

	m->jobs = (job*)realloc(m->jobs, (m->no_jobs + 1) * sizeof(job));
	m->jobs[m->no_jobs] = new_job;
	m->no_jobs += 1;

	return 0;
}


/*
 * Function: remove_job
 * --------------------
 *  Removes a job from the job array. (NO FREE)
 *
 *  pid: Pid of the job to remove from array.
 *
 *  returns: 0 on succsess, 1 if job not found.
 */
int remove_job(pid_t pid){

	int pos = 0;

	/* Find pid */
	for(int i = 0 ;i < m->no_jobs; i++){
		if(m->jobs[i].pid == pid){
			/* Remove from array */
			for(i = pos - 1; i < m->no_jobs - 1; i++){
				m->jobs[i] = m->jobs[i+1];
			}
			m->no_jobs--;
			m->jobs = (job*)realloc(m->jobs, (m->no_jobs) * sizeof(job));
			return 0;
		}
	}
	return 1;
}
