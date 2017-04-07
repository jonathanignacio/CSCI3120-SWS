/* 
 * File: rcb.c
 * Authors: Josh Caume, Jonathan Ignacio and Rhianna Goguen
 * Purpose: This file contains the rcb struct as well as any
 * 			related functions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "rcb.h"
#include "schedulers.h"

/*	This function takes a file descriptor for a client connection, parses it and returns it as an RCB
*/
struct rcb create_rcb(int fd) {
  	static char *buffer;                              		/* request buffer */
	char *req = NULL;                         	        	/* ptr to req file */
	char *brk;                                  	      	/* state used by strtok */
	char *tmp;                                      	  	/* error checking ptr */
	int len;                                          		/* length of data read */
	FILE *fin;    	                                    	/* input file handle */
	struct rcb rcblock;										/* rcb for adding to the scheduler */
	struct stat *fileinfo;									/* stat struct for getting file information*/

  	if( !buffer ) {                                   		/* 1st time, alloc buffer */
	    buffer = malloc( MAX_HTTP_SIZE );
	    if( !buffer ) {                                 	/* error check */
			perror( "Error while allocating memory" );
			abort();
	    }
	}

	memset( buffer, 0, MAX_HTTP_SIZE );
	if( read( fd, buffer, MAX_HTTP_SIZE ) <= 0 ) {   		 /* read req from client */
	    perror( "Error while reading request" );
	    abort();
	} 

	/* standard requests are of the form
	*   GET /foo/bar/qux.html HTTP/1.1
	* We want the second token (the file path).
	*/
	tmp = strtok_r( buffer, " ", &brk );              		/* parse request */
	if( tmp && !strcmp( "GET", tmp ) ) {
	    req = strtok_r( NULL, " ", &brk );
	}
	 
	if( !req ) {                                      		/* is req valid? */
	    len = sprintf( buffer, "HTTP/1.1 400 Bad request\n\n" );
	    write( fd, buffer, len );                      		/* if not, send err */
	} else {                                          		/* if so, open file */
		rcblock.fd = fd;									/* assign fd to rcb*/
	    req++;                                          	/* skip leading / */

	    fin = fopen( req, "r" );                        	/* open file */
	    rcblock.handle = fin;								/* assign file to rcb */
	    fstat(fd, fileinfo);								/* get the file info from the fd */
		rcblock.byte_remain = fileinfo->st_size;				/* remaining size starts */
		rcblock.quantum = MAX_HTTP_SIZE;					/* default max quantum to max http size */

	    if( !fin ) {                                    	/* check if successful */
			len = sprintf( buffer, "HTTP/1.1 404 File not found\n\n" );  
			write( fd, buffer, len );                     	/* if not, send err */
	    } else {                                        	/* if so, send file */
			len = sprintf( buffer, "HTTP/1.1 200 OK\n\n" );	/* send success code */
			write( fd, buffer, len );
			rcblock.success = 1;
		}
	}
	return rcblock;
}

/* Initialize an rbc table.
*/
void rcbt_init(struct rcb* table){
	struct rcb rcbt[RCBT_MAX];
	table =  rcbt;
	for(int i = 0; i < RCBT_MAX; i++){
		table[i].occupied = 0;
	}
}

/* Attempt to add an rcb to the rcb table. returns pos of entry if successful and -1 if not.
*  Takes the rcb to add as well as a pointer to the table to add it to.
*/
int rcbt_add(struct rcb input, struct rcb *table) {
	for(int i = 0; i < RCBT_MAX; i++){
		if(!table[i].occupied){
			input.pos = i;
			table[i] = input;
			return i;
		}
	}
	return 0;
}