/*

   SERVER.C
   ==========
   (c) Ashok Tamang, 2017
Email: ashok.tamang@bison.howard.edu
Networking and Web Programming, Spring 2017

-------------------------
Adapted from EchoServ by:
(c) Paul Griffiths, 1999
Email: mail@paulgriffiths.net

Simple TCP/IP echo server.

*/


#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <sys/time.h>
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


/*  Global constants  */
#define MAX_LINE           (1000)
#define LISTENQ            (1024)   /*  Backlog for listen()   */



int main(int argc, char *argv[]) {
	int       socket_tcp;                    /*  connection socket                  */
	int	      socket_udp;
	int       list_s;                /*  listening socket          */
	short int port;                  /*  port number: UDP               */
	struct    sockaddr_in servaddr;  /*  socket address structure  */
	struct sockaddr_in remaddr;  /* remote address */
	socklen_t addrlen = sizeof(remaddr); /* length of remote address */
	char      buffer[MAX_LINE];      /*  character buffer          */
	char      buffer_send[MAX_LINE];
	char     *endptr;                /*  for strtol()              */
	char     to_capitalize[MAX_LINE];         /*  store user string to capitalize */
	char     *file_name;             /*  for storing file_name to be searched in the server */
	FILE     *fp;                    /*  file pointer */


	/*  Get port number from the command line.*/

	if ( argc == 2 ) {
		port = strtol(argv[1], &endptr, 0);
		if ( *endptr ) {
			fprintf(stderr, "ECHOSERV: Invalid port number.\n");
			exit(EXIT_FAILURE);
		}
	}
	else {
		fprintf(stderr, "ECHOSERV: Invalid arguments.\n");
		printf("Usage:\n\n");
		printf("    <server> <remote Port>\n\n");
		exit(EXIT_FAILURE);
	}

	/* TCP connection */	
	/*  Create the listening socket  */

	/* if ((list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {*/
	/*perror("ECHOSERV: Error creating listening socket.\n");*/

	/*exit(EXIT_FAILURE);*/
	/*}*/


	/*  Set all bytes in socket address structure to*/
	/*zero, and fill in the relevant data members   */

	/*memset(&servaddr, 0, sizeof(servaddr));*/
	/*servaddr.sin_family      = AF_INET;*/
	/*servaddr.sin_addr.s_addr = htonl(INADDR_ANY);*/
	/*servaddr.sin_port        = htons(port);*/


	/*  Bind our socket addresss to the */
	/*listening socket, and call listen()  */

	/*if ( bind(list_s, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) {*/
	/*fprintf(stderr, "ECHOSERV: Error calling bind()\n");*/
	/*exit(EXIT_FAILURE);*/
	/*}*/

	/*if ( listen(list_s, LISTENQ) < 0 ) {*/
	/*perror("ECHOSERV: Error calling listen()\n");*/
	/*exit(EXIT_FAILURE);*/
	/*}*/

	/*UDP Connection  */

	socket_udp = socket(AF_INET, SOCK_DGRAM, 0);

	/*  Set all bytes in socket address structure to
	    zero, and fill in the relevant data members   */

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(port);


	/*  Bind our socket addresss to the 
	    listening socket, and call listen()  */

	if ( bind(socket_udp, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) {
		fprintf(stderr, "ECHOSERV: Error calling bind()\n");
		exit(EXIT_FAILURE);
	}


	/*  Enter an infinite loop to respond to client requests.  */

	while ( 1 ) {
		/*UDP connection*/
		printf("Waiting on port %d\n", port);
		int recvlen = 0;
		recvlen = recvfrom(socket_udp, buffer, MAX_LINE, 0, (struct sockaddr *) &remaddr, &addrlen);
		printf("recieved %d bytes\n", recvlen);
		if (recvlen > 0) {
			buffer[recvlen] = 0;
			printf("received message: \"%s\"\n", buffer);
		}

		/*  Wait for a connection, then accept() it  */

		/*         if ( (socket_tcp = accept(list_s, NULL, NULL) ) < 0 ) {*/
		/*fprintf(stderr, "ECHOSERV: Error calling accept()\n");*/
		/*exit(EXIT_FAILURE);*/
		/*}*/


		if (strncmp(buffer, "CAP", 3) == 0) {
			/*number of relevant bytes of message */
			/*= buffer length - 'CAP' length - length of two line breaks - end of string */

			memmove(to_capitalize, buffer + 4, strlen(buffer) - 4);
			printf("what to upper?: %s", to_capitalize);
			/*[> Capitalize the messsage <]*/
			int index = 0;
			while (to_capitalize[index] != '\0') {
				if (islower(to_capitalize[index])) {
					to_capitalize[index] = toupper(to_capitalize[index]);
				}
				else {
					to_capitalize[index] = to_capitalize[index];
				}
				index++;
			}


			/*[> parse the capitalized message to send to the client <]*/

			strcat(buffer_send, to_capitalize);

			printf("to send: %s\n", to_capitalize);
			/*[> send the formatted message to the client <]*/

			int sentlen = 0;
			sentlen = sendto(socket_udp, buffer_send, strlen(buffer_send), 0, (struct sockaddr *) &remaddr, addrlen);
			if (sentlen < 0) {
				buffer_send[sentlen] = 0;
				perror("Sending failed.");
			}
			/*[> free the memory <]*/
			to_capitalize[0] = 0;
		}

		/*if (strncmp(buffer, "FILE", 4) == 0) {*/
		/*file_name = (char* ) malloc (sizeof(char*) * (strlen(buffer) - 6));*/
		/*memcpy(file_name, buffer + 5, strlen(buffer) - 6);*/

		/*[> Find file name and read that file <]*/
		/*fp = fopen(file_name, "rb");*/
		/*if (fp) {*/
		/*long lSize;*/
		/*void* large_buffer;*/
		/*size_t result;*/

		/*[> obtain file size <]*/
		/*fseek(fp, 0, SEEK_END);*/
		/*lSize = ftell(fp);*/
		/*rewind(fp); [> Put the postion of pointer back to the start of the file <]*/

		/*[> allocate memory to hold the whole file <]*/
		/*large_buffer = (void* ) malloc(sizeof(void*) * lSize);*/
		/*if (large_buffer == NULL) {*/
		/*perror("Error in allocating required memory: ");*/
		/*}*/

		/*[> copy the file into the buffer <]*/
		/*result = fread(large_buffer, 1, lSize, fp);*/
		/*if (result != lSize) {*/
		/*printf("Error reading whole file.\n");*/
		/*}*/

		/*[> close the file and later free the large_buffer <]*/
		/*fclose(fp);*/

		/*[> send the buffer to the client <]*/
		/*if (MAX_LINE < lSize) {*/
		/*strcpy(buffer_send, "The buffer size is smaller than what needs to be sent.");*/
		/*write(socket_tcp, buffer_send, strlen(buffer_send));*/
		/*}*/
		/*else {*/
		/*sprintf(buffer_send, "%d", lSize);*/
		/*strcat(buffer_send, "\n");*/
		/*strcat(buffer_send, large_buffer);*/
		/*write(socket_tcp, buffer_send, lSize);*/
		/*}*/

		/*[> free the memory <]*/
		/*free(large_buffer);*/
		/*free(file_name);*/
		/*} else {*/
		/*[> No such file <]*/
		/*strcpy(buffer, "NOT FOUND");*/
		/*sprintf(buffer_send, "%d", strlen(buffer));*/
		/*strcat(buffer_send, "\n");*/
		/*strcat(buffer_send, buffer);*/

		/*[> Inform client that file is not in the server. <]*/
		/*write(socket_tcp, buffer_send, strlen(buffer_send));*/
		/*}*/

		/*}*/

		/*   [>* free the memory */
		memset(buffer, 0, (sizeof buffer[0]) * MAX_LINE);
		memset(buffer_send, 0, (sizeof buffer_send[0]) * MAX_LINE);

		/*  Close the connected socket  */
		/*         if ( close(socket_tcp) < 0 ) {*/
		/*perror("ECHOSERV: Error calling close()\n");*/
		/*exit(EXIT_FAILURE);*/
		/*}*/
		/*else {*/
		/*fprintf(stderr, "Connection closed.\n");*/
		/*}*/
	}
}
