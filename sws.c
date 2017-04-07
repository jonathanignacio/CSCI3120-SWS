/* 
 * File: sws.c
 * Author: Alex Brodsky
 * Modified by: Josh Caume, Jonathan Ignacio and Rhianna Goguen
 * Date: April 7, 2017
 * Purpose: This file contains the implementation of a simple web server.
 *          It consists of two functions: main() which contains the main 
 *          loop accept client connections, and serve_client(), which
 *          processes each client request.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "network.h"
#include "schedulers.h"
#include "rcb.h"

#define FCFS 0
#define SJF 1
#define RR 2
#define MLF 3

struct rcb reqtable[RCBT_MAX];		//Request table storing request control blocks for client requests
int scheduler_type;			//The type of scheduler that the server will use.

int seq_counter = 1;

/* This function takes a file handle to a client, reads in the request, 
 *    parses the request, and sends back the requested file.  If the
 *    request is improper or the file is not available, the appropriate
 *    error is sent back.
 * Parameters: 
 *             fd : the file descriptor to the client connection
 * Returns: None
 */
static void serve_client(struct rcb* input) {
  	static char *buffer;                              	/* request buffer */
	int len;                                          	/* length of data read */
	FILE *fin = input->handle;                          /* input file handle */
	int fd = input->fd;									/* file descriptor */

	                                  					/* read & send file */
    len = fread( buffer, 1, MAX_HTTP_SIZE, fin );  		/* read file chunk */
    if( len < 0 ) {                             		/* check for errors */
        perror( "Error while writing to client" );
    } else if( len > 0 ) {            	          		/* if none, send chunk */
		len = write( fd, buffer, len );
		if( len < 1 ) {                           		/* check for errors */
        perror( "Error while writing to client" );
		}
	} 
	input->byte_remain = input -> byte_remain - len;
}

/* This function is where the program starts running.
 * 	The function first parses its command line parameters to determine port #
 *	Then, it initializes, the network and enters the main loop.
 *	The main loop waits for a client (1 or more to connect, and then processes
 *	all clients by calling the seve_client() function for each one.
 * Parameters: 
 *			argc : number of command line parameters (including program name
 *			argv : array of pointers to command line parameters
 * Returns: an integer status code, 0 for success, something else for error.
 */
int main( int argc, char **argv ) {
  	int port = -1;                                    	/* server port # */
  	scheduler_type = FCFS;							    /* Defaults scheduler FCFS. */	
  	int num_threads = 1;								/* Number of threads. Defaults to 1. */	
  	int error = 0;										/* Allows full argument to be evaluated for errors.
													   	 * If error is equal to 1 there is an error. */
	struct rcb rcb_temp;								    /* temporary rcb for sending to the scheduler*/
	rcb_temp.success = 0;								//initialize rcb success as 0
	struct rcb* sjf_ptr;								//pointer for SJF scheduler
	struct rcb* rcb_p;									//pointer to an rcb in the table
	int fd;                                           	/* client file descriptor */
	int i;												//temporary int for storing position of rcb entry
/* check for and process parameters */
	if (argc < 2){										/* Ensures minimum number of arguments met. */
		error = 1;
		printf ( "usage: sms <port>\n" );
	} else {
		port = atoi(argv[1]);
		if( ( port < 1024 ) || ( port > 65525 ) ) { 	/* Ensures proper port numbers. */
			printf( "usage: sms <port>\n" );
			error = 1;
		}
		
		if ( argc > 2 ) {
			if ( strcmp(argv[2], "SJF") == 0 ) {		/* Compares argv[2] to SJF sets equal to 1 if matching. */			
			  	scheduler_type = SJF;
			  	sjf_ptr = sjf_init();
			  	if(sjf_ptr == NULL){
			  		error = 1
			  	}
			}
			else if ( strcmp(argv[2], "RR") == 0 ) {	/* "		"		"  RR 		"		 2		"	   */
			  	scheduler_type = RR;
			  	rr_init();
			}
			else if (strcmp(argv[2], "MLF") == 0 ) {	/* "		"		"  MLF		"		 3		"	   */
			  	scheduler_type = MLF;
			  	mlf_init();
			}
			else {										/* Ensures proper scheduler if required. Default FCFS. */
			  	printf( "usage: sms <scheduler>\n" );	
			  	error = 1;
			}
		}

		if ( argc > 3) {								/* Ensures valid value for number of threads. Default 1. */
			if (atoi(argv[3]) > 1)				
				num_threads = atoi(argv[3]);
			else {
				printf( "usage: sms <threads>\n" );
				error = 1;
			}
		}
	}

	if (error == 1)										//Ends program after stating errors in arguments.					
		return 0;

	rcbt_init(reqtable);								//init the rcb table
	network_init( port );                             	//init network module */


	  while(1){                                       	/* main loop */
	    network_wait();                                 /* wait for clients */

	    for( fd = network_open(); fd >= 0; fd = network_open() ) {	/* get clients */
			rcb_temp = create_rcb( fd );								/* create a rcb for the client request */
			i = rcbt_add(rcb_temp, reqtable);
			rcb_p = &reqtable[i];								//set point to point at entry in the table
			if(rcb_temp.success && i >= 0 && i < RCBT_MAX) {			/*check that the rcb was created successfully 
																		 *and that it was added to the table correctly. */
				reqtable[i].seq = seq_counter;					//assign the seq to the request and increment the counter
				i++;
				switch (scheduler_type){						//call scheduler based on given type 
	     			case (SJF):
	     				sjf_ptr = sjf_next(reqtable);					//get the next request from the updated table
	     				break;
	     			case (RR):
	     				rr_add(rcb_p);
	     				break;
	     			case (MLF):
	     				mlf_add(rcb_p);
	     				break;
	     		}

			}
			switch (scheduler_type){					/* call scheduler based on given type */
     			case (SJF):
     				rcb_p = sjf_next();						//get the next request to process from the scheduler
     				serve_client(rcb_p);
     				if(rcb_p->byte_remain <= 0){
     					rcb_p->occupied = 0;				//the request has been completed, remove it from the table
     					reqtable[rcb_p->pos] = *rcb_p;		/*update the completed empty in the table (free it) 
     														 *may not be necessary if the pointer already points 
     														 *to an entry in the table*/
     					close(rcb_p->fd);					/*close the connection for the completed request */
     				}
     				else {
     					sjf_add(rcb_p);						//if more bytes remain, add the request back to the queue
     				}
     				break;
     			case (RR) :
     				rr_add(rcb_p);
     				break;
     			case (MLF) :
     				mlf_add(rcb_p);
     				break;
	     		}
			rcb_p = NULL;
	    }
	  }
	}
