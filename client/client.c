/*
  CLIENT.C
  ==========
  (c) Ashok Tamang, 2017
  Email: ashok.tamang@bison.howard.edu
  Networking and Web Programming, Spring 2017

  -------------------------
  Adapted from EchoServ by:
  (c) Paul Griffiths, 1999
  Email: mail@paulgriffiths.net
  
  Simple TCP/IP echo client.

*/


#include <sys/socket.h>       /*  socket definitions        */
#include <netinet/in.h>
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


/*  Global constants  */

#define MAX_LINE           (1000)


/*  Function declarations  */

int ParseCmdLine(int argc, char *argv[], char **szAddress, char **szPort);


/*  main()  */

int main(int argc, char *argv[]) {

    int       conn_s;                     /*  connection socket                  */
    short int port;                       /*  port number                        */
    struct    sockaddr_in servaddr;       /*  socket address structure           */
    char      buffer[MAX_LINE];           /*  character buffer                   */
    char      buffer_send[MAX_LINE];      /*  Holds message to be send to server */
    char      buffer_received[MAX_LINE];  /*  Holds message send by server       */
    char     *szAddress;                  /*  Holds remote IP address            */
    char     *szPort;                     /*  Holds remote port                  */
    char     *endptr;                     /*  for strtol()                       */
    FILE     *fp;                         /*  file pointer                       */
    char     *file_name;                  /*  for creating and writing data into */


    /*  Get command line arguments  */

    ParseCmdLine(argc, argv, &szAddress, &szPort);


    /*  Set the remote port  */

    port = strtol(szPort, &endptr, 0);
    if ( *endptr ) {
    	printf("ECHOCLNT: Invalid port supplied.\n");
    	exit(EXIT_FAILURE);
    }
	

    /*  Create the listening socket  */

    if ( (conn_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
    	fprintf(stderr, "ECHOCLNT: Error creating listening socket.\n");
    	exit(EXIT_FAILURE);
    }

    /*  Set all bytes in socket address structure to
        zero, and fill in the relevant data members   */

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons(port);


    /*  Set the remote IP address  */

    if ( inet_aton(szAddress, &servaddr.sin_addr) <= 0 ) {
    	printf("ECHOCLNT: Invalid remote IP address.\n");
    	exit(EXIT_FAILURE);
    }

    
    /*  connect() to the remote echo server  */

    if ( connect(conn_s, (struct sockaddr *) &servaddr, sizeof(servaddr) ) < 0 ) {
    	printf("ECHOCLNT: Error calling connect()\n");
    	exit(EXIT_FAILURE);
    }


    /*  Get string to follow user commands */
    do {
        printf("Insert your command: ");
        fgets(buffer, MAX_LINE, stdin);

        if (strncmp(buffer, "s", 1) == 0) {
            printf("\nPlease Enter a string: ");
            fgets(buffer, MAX_LINE, stdin);
            
            /* Format the input string */
            strcpy(buffer_send, "CAP\n");
            strcat(buffer_send, buffer);
            strcat(buffer_send, "\n");

            write(conn_s, buffer_send, strlen(buffer_send));
            read(conn_s, buffer_received, MAX_LINE-1);

            /* reset buffer to get only relevant string */
            memset(buffer, 0, (sizeof buffer[0]) * MAX_LINE);
            
            memcpy(buffer, buffer_received + 2, strlen(buffer_received) - 2);
            printf("Server responded: %s", buffer);
        }
        else if (strncmp(buffer, "t", 1) == 0) {
            printf("\nPlease Enter a string: ");
            fgets(buffer, MAX_LINE, stdin);
            
            file_name = (char*) malloc (sizeof(char*) * (strlen(buffer) - 1));
            memcpy(file_name, buffer, strlen(buffer) - 1);  

            /* Format the input string */
            strcpy(buffer_send, "FILE\n");
            strcat(buffer_send, buffer);

            /* Send message to server. */
            write(conn_s, buffer_send, strlen(buffer_send));

            /* Read message from server. */
            read(conn_s, buffer_received, MAX_LINE-1);
 
            /* write the data to the file. */
            if (strncmp(buffer_received + 2, "NOT FOUND", 9) == 0) {
                printf("Server responded: %s\n", buffer_received + 2);
            }
            else {
                fp = fopen(file_name, "wb");
                fwrite(buffer_received, 1, strlen(buffer_received), fp);
                printf("Server responded: Data is written to the file named: %s\n", file_name);
                /* close the file and free the memory */
                fclose(fp);
                free(file_name);
            }
        }
        else if (strncmp(buffer, "q", 1) == 0) {
            fprintf(stderr, "Now should exit.\n");
            if (close(conn_s) < 0 ) {
                fprintf(stderr, "ECHOSERV: Error calling close()\n");
                exit(EXIT_FAILURE);
            }

            return EXIT_SUCCESS;
        }
        else 
            printf("\nInvalid Command: Press 's' for echo, 't' for file storage and 'q' for exit.\n");

        /* free the memory */
        memset(buffer, 0, (sizeof buffer[0]) * MAX_LINE);
        memset(buffer_send, 0, (sizeof buffer_send[0]) * MAX_LINE);
        memset(buffer_received, 0, (sizeof buffer_received[0]) * MAX_LINE);
    
    } while (strncmp(buffer, "q", 1) != 0);

    return EXIT_SUCCESS;
}


int ParseCmdLine(int argc, char *argv[], char **szAddress, char **szPort) {
    /* Number of arguments: <clent> <ip> <port> (3 arguments). */
    if (argc == 3) {
        *szAddress = argv[1];
	    *szPort = argv[2];
    }
	else {
	    printf("Usage:\n\n");
	    printf("    <client> <IP address> <remote Port>\n\n");
	    exit(EXIT_SUCCESS);
	}
    return 0;
}

