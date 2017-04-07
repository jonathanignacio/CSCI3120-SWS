/* 
 * File: schedulers.h
 * Authors: Josh Caume, Jonathan Ignacio and Rhianna Goguen
 * Purpose: This file contains the functions for different 
 * 			schedulers for the sws. Has SJF, RR and MLFQ.
 */

#ifndef SCHEDULERS_H
#define SCHEDULERS_H


#define MAX_HTTP_SIZE 8192                 	//size of buffer to allocate 

#include "rcb.h"

/*******************Shortest Job First Functions*****************************/

void sjf_init();

void sjf_add(struct rcb* input);

struct rcb* sjf_next();

/*******************Round Robin Functions************************************/

void rr_init();

void rr_add(struct rcb* input);

struct rcb* rr_next();

/*******************Multilevel Feedback Queue Functions**********************/

void mlf_init();

void mlf_add(struct rcb* input);

struct rcb* mlf_next();

#endif