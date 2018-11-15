#include "mysh.h"

#define SetBit(BM,n)     (BM[(n/8)] |= (1 << (n%8)))
#define ClearBit(BM,n)   (BM[(n/8)] &= ~(1 << (n%8)))
#define TestBit(BM,n)    (BM[(n/8)] & (1 << (n%8)))


/*
 * Function: 
 * --------------------
 * 	Searches for 'n' free bit.
 *
 *  *bitmap: bitmap to search in
 *  n: number of bits needed
 *  returns: 1 if 'n' bits are free, 0 if not.
 */
int test_n_free_bit(unsigned char *bitmap, int n){
	int free = 0;
	for(int i = 0; i < MAX_BLOCKS; i++){
		if(!TestBit(bitmap, i)){
			free++;
			if(free >= n){
				return 1;
			}
		}
	}
	return 0;
}


/*
 * Function: 
 * --------------------
 * 	Searches for a free bit.
 *
 *  *bitmap: bitmap to search in
 *  returns: index of bit, or 1 if all are allocated.
 */
int get_free_bit(unsigned char *bitmap){
	for(int i = 0; i < MAX_BLOCKS; i++){
		if(!TestBit(bitmap, i)){
			return i;
		}
	}
	return 1;
}


/*
 * Function: 
 * --------------------
 * 	Sets the 'i' bit.
 *
 *  *bitmap: bitmap to search in
 *  i: index of bit to set
 */
void set_bit(unsigned char *bitmap, int i){
	SetBit(bitmap, i);
}

/*
 * Function: 
 * --------------------
 * 	Clears the 'i' bit.
 *
 *  *bitmap: bitmap to search in
 *  i: index of bit to clear
 */
void free_bit(unsigned char *bitmap, int i){
	ClearBit(bitmap, i);
}
