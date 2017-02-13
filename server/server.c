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
    short int udp_port;                  /*  port number: UDP               */
    short int tcp_port;              /*  port number: TCP          */
    struct    sockaddr_in servaddr_udp;  /*  socket address structure  */
    struct    sockaddr_in servaddr_tcp;
    struct sockaddr_in remaddr;  /* remote address */
    socklen_t addrlen = sizeof(remaddr); /* length of remote address */
    char     *buffer;      /*  character buffer          */
    char     *buffer_send;
    char     *endptr;                /*  for strtol()              */
    char     *to_capitalize;         /*  store user string to capitalize */
    char     *file_name;             /*  for storing file_name to be searched in the server */
    char     *temp;                   /*  for temporary storage of strings; for formatting */
    FILE     *fp;                    /*  file pointer */


    /*  Get port number from the command line.*/

    if ( argc == 2 ) {
        memset(&endptr, 0, sizeof(endptr)); /* Reset endptr for udp_port */
        udp_port = strtol(argv[1], &endptr, 0);
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

    /*UDP Connection  */

    socket_udp = socket(AF_INET, SOCK_DGRAM, 0);

    /*  Set all bytes in socket address structure to
        zero, and fill in the relevant data members   */

    memset(&servaddr_udp, 0, sizeof(servaddr_udp));
    servaddr_udp.sin_family      = AF_INET;
    servaddr_udp.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr_udp.sin_port        = htons(udp_port);


    /*  Bind our socket addresss to the 
        listening socket, and call listen()  */

    if ( bind(socket_udp, (struct sockaddr *) &servaddr_udp, sizeof(servaddr_udp)) < 0 ) {
        fprintf(stderr, "ECHOSERV: Error calling bind()\n");
        exit(EXIT_FAILURE);
    }


    /*  Enter an infinite loop to respond to client requests.  */

    while ( 1 ) {
        /*UDP connection*/
        printf("Waiting on port %d\n", udp_port);
        int recvlen = 0;
        buffer = (char *) malloc(sizeof(buffer) * MAX_LINE);
        recvlen = recvfrom(socket_udp, buffer, MAX_LINE, 0, (struct sockaddr *) &remaddr, &addrlen);
        printf("recieved %d bytes\n", recvlen);
        if (recvlen > 0) {
            buffer[recvlen] = '\0';
            printf("received message: \"%s\"\n", buffer);
        }
        printf("buffer len: %d\n.", strlen(buffer));

        if (strncmp(buffer, "CAP", 3) == 0) {
            /*number of relevant bytes of message */
            /*= buffer length - 'CAP' length - length of two line breaks - end of string */
            to_capitalize = (char *) malloc(sizeof(to_capitalize) * strlen(buffer));
            strncpy(to_capitalize, buffer + 4, recvlen - 4);
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
            printf("to send: %s\n", to_capitalize);
            printf("to send length: %d\n", strlen(to_capitalize));
            /*[> send the formatted message to the client <]*/
            int sentlen = 0;

            /*strlen doesn't return the length including null terminator.*/
            sentlen = sendto(socket_udp, to_capitalize, strlen(to_capitalize), 0, (struct sockaddr *) &remaddr, addrlen);
            if (sentlen < 0) {
                perror("Sending failed.");
            }
            /*[> free the memory <]*/
            free(to_capitalize);
            free(buffer);
        }

        if (strncmp(buffer, "FILE", 4) == 0) {

            /*Allocate memory*/
            file_name = (char *) malloc (sizeof(char*) * recvlen);
            /*tcp_port = (s<]hort int *) malloc (sizeof(tcp_port) * 4); [> 4 digits for port number. */

            /*Temp pointer to hold string tcp port.*/
            char *string_port;
            string_port = (char *) malloc(sizeof(string_port) * 10);

            char *token; /*for parsing the message into file name and port */
            const char delim = '\n';
            token = (char *) malloc(sizeof(token) * strlen(buffer)+1);
            strcpy(token, buffer);
            token = strtok(token, "\n"); 

            /*Walk through other tokens.*/
            int position = 0; /* Position: 1 = FILE, 2 = <file name>, 3 = TCP port */
            while (token != NULL) {
                if (position == 1)
                    strcpy(file_name, token);
                if (position == 2)
                    strcpy(string_port, token);
                if (position > 2) 
                    break;
                position++;
                token = strtok(NULL, "\n");
            }

            free(token);


            /*Check if the file exists.*/
            printf("Search file: %s.\n", file_name);
            printf("TCP port in string: %s\n", string_port);

            /* Find file name and read that file */
            fp = fopen(file_name, "rb");
            if (fp) {

                printf("Do you even go here?\n");
                /*Read the file and file size.*/
                long lSize;
                void* large_buffer;
                size_t result;

                /* obtain file size */
                fseek(fp, 0, SEEK_END);
                lSize = ftell(fp);
                rewind(fp); /* Put the postion of pointer back to the start of the file */

                /*Inform client that file exists.*/

                /*Format status message.*/

                temp = (char *) malloc(sizeof(temp) * MAX_LINE);
                buffer_send = (char *) malloc(sizeof(buffer_send) * MAX_LINE);

                sprintf(temp, "%Ld", lSize);
                printf("size of file: %Ld\n", lSize);
                strcpy(buffer_send, "OK\n");
                strcat(buffer_send, temp);
                strcat(buffer_send, "\n");

                printf("stats ok mesg: %s\n", buffer_send); /*debug*/

                /*Send Status message to client.*/
                if (sendto(socket_udp, buffer_send, strlen(buffer_send), 0, (struct sockaddr *) &remaddr, addrlen) < 0) {
                    perror("Failed to send status.");
                    exit(EXIT_FAILURE);
                }


                /* allocate memory to hold the whole file */
                large_buffer = (void* ) malloc(sizeof(void*) * lSize);
                if (large_buffer == NULL) {
                    perror("Error in allocating required memory: ");
                }

                /* copy the file into the buffer */
                result = fread(large_buffer, 1, lSize, fp);
                if (result != lSize) {
                    printf("Error reading whole file.\n");
                }

                /* close the file and later free the large_buffer */
                fclose(fp);

                /*Set up TCP connection*/
                /*Create the listening socket  */

                /*Free endptr before using*/
                /*free(endptr);*/

                memset(&endptr, 0, sizeof(endptr)); /* Reset endptr for tcp_port */
                tcp_port = strtol(string_port, &endptr, 0);
                /*free(string_port);*/
                if ( *endptr ) {
                    printf("ECHOCLNT: Invalid port supplied.\n");
                    exit(EXIT_FAILURE);
                }
                if ((list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
                    perror("ECHOSERV: Error creating listening socket.\n");

                    exit(EXIT_FAILURE);
                }


                /*Set all bytes in socket address structure to*/
                /*zero, and fill in the relevant data members   */

                memset(&servaddr_tcp, 0, sizeof(servaddr_tcp));
                servaddr_tcp.sin_family      = AF_INET;
                servaddr_tcp.sin_port        = htons(tcp_port);
                servaddr_tcp.sin_addr.s_addr = htonl(INADDR_ANY);

                /*Bind our socket addresss to the */
                /*listening socket, and call listen()  */

                if ( bind(list_s, (struct sockaddr *) &servaddr_tcp, sizeof(servaddr_tcp)) < 0 ) {
                    perror("Error calling bind()\n");
                    exit(EXIT_FAILURE);
                }

                if ( listen(list_s, LISTENQ) < 0 ) {
                    perror("ECHOSERV: Error calling listen()\n");
                    exit(EXIT_FAILURE);
                }

                while (1) {
                    /*Wait for a connection, then accept() it  */

                    if ( (socket_tcp = accept(list_s, NULL, NULL) ) < 0 ) {
                        perror("Error in calling accept():");
                        exit(EXIT_FAILURE);
                    }

                    write(socket_tcp, large_buffer, lSize); 
                    break;
                }

                /*Close socket_tcp.*/
                if (close(socket_tcp) < 0) {
                    perror("Error calling close()\n");
                    exit(EXIT_FAILURE);
                }

                /*Close list_s.*/
                if (close(list_s) < 0) {
                    perror("Error calling close()\n");
                    exit(EXIT_FAILURE);
                }
                else {
                    printf("Transfer complete.\n");
                }

                int opt = 1;
                setsockopt(socket_tcp, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
                setsockopt(list_s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

                
                /* free memory from local pointers */
                free(temp);
                free(buffer_send);
                free(large_buffer);
            }
            else {
                /*Inform client that file doesn't exist*/
                buffer_send = (char *) malloc(sizeof(buffer_send) * MAX_LINE);
                strcpy(buffer_send, "NOT FOUND\n");

                printf("to send: %s\n", buffer_send);
                printf("to send len: %d\n", strlen(buffer_send));

                /*Send message to client.*/

                if (sendto(socket_udp, buffer_send, strlen(buffer_send), 0, (struct sockaddr *) &remaddr, addrlen) < 0) {
                    perror("Failed to send no file status.");
                    exit(EXIT_FAILURE);
                }


            }

            free(file_name);
            /*free(tcp_port);*/
        }
    }
}

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

/*  Close the connected socket  */
/*         if ( close(socket_tcp) < 0 ) {*/
/*perror("ECHOSERV: Error calling close()\n");*/
/*exit(EXIT_FAILURE);*/
/*}*/
/*else {*/
/*fprintf(stderr, "Connection closed.\n");*/
/*}*/

