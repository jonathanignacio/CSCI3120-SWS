/* 
 * File: schedulers.c
 * Authors: Josh Caume, Jonathan Ignacio and Rhianna Goguen
 * Purpose: This file contains the functions for different 
 * 			schedulers for the sws. Has SJF, RR and MLFQ.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "schedulers.h"

/*In general, the schedulers will store references to entries in the table
 *to determine their priority and processing order*/
/*******************Shortest Job First Functions*****************************/

/* Returns a pointer to be used as reference to the smallest entry
*/
struct rcb *sjf_init() {
	struct rcb *temp;
	temp = (struct rcb *)malloc(sizeof(struct rcb)); //dynamically allocate the pointer for future use

		if (temp == 0)
		{
			printf("Error, unable to initialize SJF scheduler");
			return NULL;
		}

	temp->occupied = 0; 							//initialize the ptr to "not occupied"
	return temp;
}

struct rcb *sjf_next(struct rcb *table, struct rcb *ptr) {
	unsigned int lowest = ptr->byte_remain;
	char set = 0;											//variable for determining if the ptr has valid values
	if (ptr->occupied = 1){
		lowest = ptr->byte_remain;
		set = 1; 											//flag that the entry was set properly
	}
	else {
		for(int i = 0; i < RCBT_MAX; i++){
			if(table[i].occupied) {	 						//set the ptr to the first occupied entry
				ptr = &table[i];
				i = RCBT_MAX;
				set = 1;									//flag that the entry was set properly
			}
		}
	}
	if(set){												//check if the ptr has been set to a valid entry
		for(int i = 0; i < RCBT_MAX; i++){
			if(table[i].occupied && table[i].byte_remain < ptr->byte_remain) {	 //if an occupied entry is lower than ptr, then set ptr to that entry
				ptr = &table[i];	
			}
		}
	}
	else{
		printf("Error, no entries found in the request table");
		return NULL;
	}

	return ptr;
}

/*******************Round Robin Functions************************************/

void rr_init() {

}

void rr_add(struct rcb* input) {

}

struct rcb* rr_next() {
	struct rcb temp; //does nothing yet
	return &temp;
}

/*******************Multilevel Feedback Queue Functions**********************/

void mlf_init() {

}

void mlf_add(struct rcb* input) {

}

struct rcb* mlf_next() {
	struct rcb temp; //does nothing yet
	return &temp;
}