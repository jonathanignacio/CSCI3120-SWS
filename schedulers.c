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

void sjf_init() {

}

void sjf_add(struct rcb* input) {

}

struct rcb* sjf_next() {
	struct rcb temp; //does nothing yet
	return &temp;
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