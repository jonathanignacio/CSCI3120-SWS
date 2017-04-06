/* 
 * File: sws.c
 * Author: Alex Brodsky
 * Purpose: This file contains the implementation of a simple web server.
 *          It consists of two functions: main() which contains the main 
 *          loop accept client connections, and serve_client(), which
 *          processes each client request.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "network.h"

#define MAX_REQ 255							//maximum requests on the request table
#define MAX_HTTP_SIZE 8192                 /* size of buffer to allocate */
#define FCFS 0
#define SJF 1
#define RR 2
#define MLF 3


/* This function takes a file handle to a client, reads in the request, 
 *    parses the request, and sends back the requested file.  If the
 *    request is improper or the file is not available, the appropriate
 *    error is sent back.
 * Parameters: 
 *             fd : the file descriptor to the client connection
 * Returns: None
 */

struct rcb //Request control block for a client request
{
	int seq; 			//The sequence number of the request
	int fd; 			//The file descriptor of the client (returned by network_wait())
	FILE * handle;		//Handle of file being requested
	int byte_remain; 	//Number of bytes of the file that remain to be sent
	int quantum;		//Maximum number of bytes to be sent when the request is serviced
}

rcb[255] reqtable;		//Request table storing request control blocks for client requests

static void serve_client( int fd ) {
  	static char *buffer;                              		* request buffer */
		char *req = NULL;                                 	/* ptr to req file */
		char *brk;                                        	/* state used by strtok */
		char *tmp;                                        	/* error checking ptr */
		FILE *fin;                                        	/* input file handle */
		int len;                                          	/* length of data read */

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
	    req++;                                          	/* skip leading / */
	    fin = fopen( req, "r" );                        	/* open file */
	    if( !fin ) {                                    	/* check if successful */
			len = sprintf( buffer, "HTTP/1.1 404 File not found\n\n" );  
			write( fd, buffer, len );                     	/* if not, send err */
	    } else {                                        	/* if so, send file */
			len = sprintf( buffer, "HTTP/1.1 200 OK\n\n" );	/* send success code */
			write( fd, buffer, len );

		do {                                          		/* loop, read & send file */
	        len = fread( buffer, 1, MAX_HTTP_SIZE, fin );  	/* read file chunk */
	        if( len < 0 ) {                             	/* check for errors */
	            perror( "Error while writing to client" );
	        } else if( len > 0 ) {                      	/* if none, send chunk */
				len = write( fd, buffer, len );
				if( len < 1 ) {                           	/* check for errors */
	            perror( "Error while writing to client" );
				}
	        }
		} while( len == MAX_HTTP_SIZE );              		/* the last chunk < 8192 */
      	fclose( fin );
	    }
	}
	close( fd );                                     		/* close client connectuin*/
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
  	int fd;                                           	/* client file descriptor */
  	int scheduler_type = FCFS;							/* Type of scheduler to be used. Defaults as FCFS. */	
  	int num_threads = 1;								/* Number of threads. Defaults to 1. */	
  	int error = 0;										/* Allows full argument to be evaluated for errors.
													   	 *	If error is equal to 1 there is an error. */
/* check for and process parameters */
	if (argc < 2){										/* Ensures minimum number of arguments met. */
		error = 1;
		printf ( "usage: sms <port>\n" );
	} else {
		port = atoi(arv[1]);
		if( ( port < 1024 ) || ( port > 65525 ) ) { 	/* Ensures proper port numbers. */
			printf( "usage: sms <port>\n" );
			error = 1;
		}
		
		if ( argc > 2 ) {
			if ( strcmp(argv[2], "SJF") == 0 )			/* Compares argv[2] to SJF sets equal to 1 if matching. */			
			  	scheduler_type = SJF;
			else if ( strcmp(argv[2], "RR") == 0 )		/* "		"		"  RR 		"		 2		"	   */
			  	scheduler_type = RR;
			else if (strcmp(argv[2], "MLF") == 0 )		/* "		"		"  MLF		"		 3		"	   */
			  	scheduler_type = MLF;
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

	if (error == 1)										/* Ends program after stating errors in arguments. */					
		return 0;

	  network_init( port );                             /* init network module */

	  while(1){                                       	/* main loop */
	    network_wait();                                 /* wait for clients */

	    for( fd = network_open(); fd >= 0; fd = network_open() ) {	/* get clients */
	     	serve_client( fd );                      	/* process each client */
	    }
	  }
	}
