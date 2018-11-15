#include "mysh.h"


/*
 * Function: push
 * ----------------------------
 *   Adds an element at the start of the list.
 *
 *   *head: head of the list
 *   len: lenght of the history input
 *   d_i: Array of block indexes where the command is stored.
 *
 */
void push(md **head, int len, int *d_i) {

	md *new_block;
	new_block = malloc(sizeof(md));

	new_block->len = len;
	for(int i = 0; i < 15; i++){
		new_block->d_index[i] = d_i[i];
	}
	new_block->next = *head;

	*head = new_block;
}


/*
 * Function: pop
 * ----------------------------
 *   Removes and returns a pointer to the last element in the list.
 *
 *   *head: head of the list
 *
 */
md *pop(md *head){

    if (head->next == NULL) {
	        free(head);
	}

    md *current = head;
    while(current->next->next != NULL){
	        current = current->next;
	}

	md *ret = current->next;
    current->next = NULL;
	return ret;
}


/*
 * Function: remove_all
 * ----------------------------
 *   Iterates over the whole linked list and removes and frees each element.
 *
 *   *head: head of the list
 *
 */
void remove_all(md *head){
	md *current = head;
	md *current_n;

	while(current != NULL){
		current_n = current->next;
		free(current);
		current = current_n;
	}
}


/*
 * Function: remove_n
 * ----------------------------
 *   Removes the 'n' element from the list and returns a pointer to it.
 *
 *   *head: head of the list
 *   n: n'th element to remove from list
 *
 *   returns: A pointer to the element.
 */
md *remove_n(md *head, int n){

    int i = 0;
    md * current = head;
    md * temp_node = NULL;

    if (n == 0){
		md next_node = *head->next;
		free(head);
		*head = next_node;
    }

    for (i = 0; i < n-1; i++) {
        if (current->next == NULL) {
        }
        current = current->next;
    }

    temp_node = current->next;
    current->next = temp_node->next;

	return temp_node;
}
