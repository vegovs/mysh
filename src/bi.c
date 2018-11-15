#include "mysh.h"

/* Shell struct */
extern mysh *m;
/* Function pointer to commands */
extern char *builtins_cmds[];
/* Data handling struct for history */
extern h_mem h_m;


/*
 * Function: mysh_quit
 * --------------------
 *  Return -1 to quit or 1 on bad usage.
 *
 *  **args: 'none'
 */
int mysh_quit(char **args) {

	/* Usage */
	char *usage = "usage: type <cmd>\n 	cmd: command\n";
	int argc = 0;
	for(int i = 0;args[i]; i++){
		argc++;
	}

	if(argc != 1){
		printf("%s\n", usage);
		return 1;
	}

	/* Quit */
	return -1;
}


/*
 * Function: mysh_type
 * --------------------
 *  Displays information about command type.
 *
 *  **args: command
 *
 *  returns: 0 if argument is of type built in, 1 on bad usage or command not found.
 */
int mysh_type(char **args) {

	/* Usage */
	char *usage = "usage: type <cmd>\n 	cmd: command\n";
	int argc = 0;
	for(int i = 0;args[i]; i++){
		argc++;
	}

	int i;

	if(argc != 2){
		printf("%s", usage);
		return 1;
	}

	/* Compare to builtins */
	for(i = 0; i < NO_BUILTINS; i++){
	    if(strcmp(args[1], builtins_cmds[i]) == 0){
			printf("%s is a shell builtin\n", args[1]);
			return 0;
		}
	}

	printf("mysh: type: %s: not found\n", args[1]);
	return 1;
}


/*
 * Function: mysh_h
 * ----------------------------
 *   Prints command history, executes command 'i' or deletes history line 'i' depending on arguments.
 *
 *   **args: Se usage
 *
 *   usage: h [-d <i>] <i>
 *   	-d i: Delete history input 'i'
 *   	i: Run history input i
 *
 *   returns: 0 on success, 1 on usage error.
 */
int mysh_h(char **args){

	/* Usage */
	char *usage = "usage: h [-d <i>] <i>\n 	-d i: Delete history input 'i'\n 	i: Run history input i\n";
	int argc = 0;
	for(int i = 0;args[i]; i++){
		argc++;
	}

	/* History blocks array */
	char *cmds[MAX_BLOCKS];
	/* Head of metadatblock linked list */
	md *current = m->head;
	/* History index */
	int  index = 0;

	/* Get commands */
	while(current != NULL){
		char line[INPUT_BUFSIZE];
		line[0] = '\0';
		for(int i = 0; current->d_index[i] >= 0; i++){
			strncat(line, &h_m.hist[current->d_index[i]*8], 8);
		}
		cmds[++index] = strdup(line);
		current = current->next;
	}

	/* Print history */
	if(argc == 1){
		printf("\nHistory list of the last %d commands:\n", index);
		for(int i = index; i > 0; i--){
			printf("%3d: %s\n", i, cmds[i]);
			free(cmds[i]);
		}
		return 1;
	}

	/* Delete history input */
	if(argc == 3 && (strcmp(args[1], "-d") == 0)){
		int i = atoi(args[2]);
		if((i > 0) && (i < index)){
			/* Get metadatblock */
			md *delete_me = remove_n(m->head, i);
			/* Clear bits and free memory */
			for(int j = 0; delete_me->d_index[j] >= 0; j++){
				free_bit(h_m.bm, delete_me->d_index[j]);
				int hist_index = delete_me->d_index[j]*BLOCK_SIZE;
				memset(&h_m.hist[hist_index], '\0', BLOCK_SIZE);
				delete_me->next = NULL;
			}

			/* Free cmds */
			for(int i = index; i > 0; i--){
				free(cmds[i]);
			}
			index--;
			free(delete_me);
#ifdef DEBUG
			debug_bitmap(h_m.bm);
			debug_datablocks(h_m.hist);
#endif
			return 1;
		}
	}

	/* Run history input */
	if(argc == 2){
		int ret = 0;
		int i = atoi(args[1]);
		/* Usage control */
		if((i > 0) && (i < index)){
			char *param[PARAMS_BUFSIZE];
			int no_params = strtok_param(cmds[i+1], param);
			ret = param_parser(param, no_params);
			/* Free cmds */
			for(int i = index; i > 0; i--){
				free(cmds[i]);
			}
			return ret;
		}
	}

	/* Free cmds */
	for(int i = index; i > 0; i--){
		free(cmds[i]);
	}

	printf("%s", usage);
	return 1;
}


/*
 * Function: mysh_jobs
 * ----------------------------
 *   Prints current running jobs.
 *
 *   **args: 'none'
 *
 *   returns: 0 on success, 1 on usage error.
 */
int mysh_jobs(char **args){

	char *usage = "usage: jobs\n";
	int argc = 0;
	for(int i = 0;args[i]; i++){
		argc++;
	}

	if(argc != 1){
		printf("%s", usage);
		return 1;
	}

	for(int i = 0 ; i < m->no_jobs; i++){
		printf("\nPid 			= %d", m->jobs[i].pid);
	    printf("\nCommand line 		= %s\n", m->jobs[i].cmd);
	}
	return 0;
}


/*
 * Function: mysh_kill
 * ----------------------------
 *   Kills the running process with.
 *
 *   **args: pid of the process to kill
 *
 *   returns: 0 on success, 1 on usage error, -1 on other error or process not found.
 */
int mysh_kill(char **args){

	/* Usage */
	char *usage = "usage: kill <i>\n 	i: pid of job to kill\n";
	int argc = 0;
	for(int i = 0;args[i]; i++){
		argc++;
	}

	if(argc != 2){
		printf("%s", usage);
		return 1;
	}

	/* Pid to kill */
	pid_t kill_pid = atoi(args[1]);

	/* Pisition in jobs-array */
	int pos = 1;

	/* Find pid */
	for(int i = 0 ;i < m->no_jobs; i++){
		if(m->jobs[i].pid == kill_pid){
			/* Kill */
			if(kill(kill_pid, SIGKILL) == -1){
				fprintf(stderr, "ERROR: Could not kill (%d)\n", kill_pid);
				perror("Error: ");
				return -1;
			}
			/* Remove from jobs array */
			remove_job(kill_pid);
			return 0;
		}
		pos++;
	}

	printf("bash: kill: (%d) - No such process\n", kill_pid);
	return -1;
}
