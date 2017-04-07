/* 
 * File: rcb.h
 * Authors: Josh Caume, Jonathan Ignacio and Rhianna Goguen
 * Purpose: This file contains the rcb struct as well as any
 * 			related functions.
 */

#ifndef RCB_H
#define RCB_H
#define RCBT_MAX 100 //rcb table can have a maximum number of entries
#endif


struct rcb //Request control block for a client request
{
	char occupied;		//variable to determine if a space on the rcb table is occupied. 1 for occupied
	char success; 		//variable to determine if a request was successfully converted from an fd 
	int seq; 			//The sequence number of the request
	int pos				//The position of the rcb in the table
	int fd; 			//The file descriptor of the client (returned by network_wait())
	FILE * handle;		//Handle of file being requested
	unsigned int byte_remain; //Number of bytes of the file that remain to be sent
	int quantum;		//Maximum number of bytes to be sent when the request is serviced
}

/*	This function takes a file descriptor for a client connection, parses it and returns it as an RCB
*/
rcb* create_rcb(int fd);
