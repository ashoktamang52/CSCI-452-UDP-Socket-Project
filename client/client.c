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

int ParseCmdLine(int argc, char *argv[], char **szAddress, char **tcpPort, char **udpPort);


/*  main()  */

int main(int argc, char *argv[]) {

    int       socket_tcp;                    /*  connection socket                  */
    int	      socket_udp;
    short int tcp_port;                   /*  tcp port number                        */
    short int udp_port;                   /*  udp port number                    */
    struct    sockaddr_in servaddr;       /*  socket address structure           */
    struct    sockaddr_in remaddr;        /*  remote address               	     */
    socklen_t addrlen = sizeof(remaddr);  /*  length of remote address 	     */
    char     *buffer;           /*  character buffer                   */
    char     *buffer_send;      /*  Holds message to be send to server */
    char     *buffer_received;  /*  Holds message send by server       */
    char     *szAddress;                  /*  Holds remote IP address            */
    char     *udpPort;			 /* Holds server upd port */
    char     *tcpPort;                     /*  Holds server tcp port                  */
    char     *endptr;                     /*  for strtol()                       */
    FILE     *fp;                         /*  file pointer                       */
    char     *file_name;                  /*  for creating and writing data into */


    /*  Get command line arguments  */

    ParseCmdLine(argc, argv, &szAddress, &tcpPort, &udpPort);


    /* TCP connection */

    /*  Set the remote port  */

    /*tcp_port = strtol(tcpPort, &endptr, 0);*/
    /*if ( *endptr ) {*/
    /*printf("ECHOCLNT: Invalid port supplied.\n");*/
    /*exit(EXIT_FAILURE);*/
    /*}*/


    /*[>  Create the listening socket  <]*/

    /*if ( (socket_tcp = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {*/
    /*fprintf(stderr, "ECHOCLNT: Error creating listening socket.\n");*/
    /*exit(EXIT_FAILURE);*/
    /*}*/

    /*  Set all bytes in socket address structure to*/
    /*zero, and fill in the relevant data members   */

    /*memset(&servaddr, 0, sizeof(servaddr));*/
    /*servaddr.sin_family      = AF_INET;*/
    /*servaddr.sin_port        = htons(tcp_port);*/

    /*[>  Set the remote IP address  <]*/

    /*if ( inet_aton(szAddress, &servaddr.sin_addr) <= 0 ) {*/
    /*printf("ECHOCLNT: Invalid remote IP address.\n");*/
    /*exit(EXIT_FAILURE);*/
    /*}*/


    /*[>  connect() to the remote echo server  <]*/

    /*if ( connect(socket_tcp, (struct sockaddr *) &servaddr, sizeof(servaddr) ) < 0 ) {*/
    /*perror("Connection failed");*/
    /*exit(EXIT_FAILURE);*/
    /*}*/

    /* UDP connection */

    /*  Set the remote port  */
    memset(&endptr, 0, sizeof(endptr)); /* Reset endptr for udp_port */
    udp_port = strtol(udpPort, &endptr, 0);
    if ( *endptr ) {
        printf("ECHOCLNT: Invalid port supplied.\n");
        exit(EXIT_FAILURE);
    }

    /*Create UDP socket*/

    if ( (socket_udp = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        fprintf(stderr, "ECHOCLNT: Error creating UDP socket.\n");
        exit(EXIT_FAILURE);
    }

    /*  Set all bytes in socket address structure to
        zero, and fill in the relevant data members   */

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons(udp_port);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    /*  Get string to follow user commands */
    do {
        buffer = (char *) malloc(sizeof(buffer) * MAX_LINE);
        printf("Insert your command: ");
        fgets(buffer, MAX_LINE, stdin);

        if (strncmp(buffer, "s", 1) == 0) {
            /*Reset buffer to get the string.*/
            free(buffer);
            buffer = (char *) malloc(sizeof(buffer) * MAX_LINE);

            printf("\nPlease Enter a string: ");
            fgets(buffer, MAX_LINE, stdin);

            /*CAP + 2 new lines + null terminator = 6*/
            buffer_send =  (char *) malloc(sizeof(buffer_send) * (strlen(buffer))); 
            printf("size of buffer: %d\n", strlen(buffer));
            /* Format the input string */
            strcpy(buffer_send, "CAP\n");
            strcat(buffer_send, buffer);

            printf("to send length: %d\n", strlen(buffer_send));
            /*Send message to server via UDP*/
            if (sendto(socket_udp, buffer_send, strlen(buffer_send), 0, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
                perror("Send to, failed.");
                exit(EXIT_FAILURE);
            }

            /*Read message back from server via UDP*/
            int recvlen = 0;
            buffer_received = (char *) malloc(sizeof(buffer_received) * MAX_LINE);
            recvlen = recvfrom(socket_udp, buffer_received, MAX_LINE, 0, (struct sockaddr *) &remaddr, &addrlen);
            if (recvlen > 0) {
                buffer_received[recvlen] = '\0';
                printf("total bytest received: %d.\n", recvlen);
                printf("Server responded: %s\n", buffer_received);
            }


            /* reset buffer to get only relevant string */
            free(buffer);
            free(buffer_received);
            free(buffer_send); 
            /*printf("Server responded: %s", buffer);*/
        }
        else if (strncmp(buffer, "t", 1) == 0) {
            /*Reset buffer to get the string.*/
            free(buffer);
            buffer = (char *) malloc(sizeof(buffer) * MAX_LINE);

            printf("\nPlease Enter a string: ");
            fgets(buffer, MAX_LINE, stdin);

            file_name = (char*) malloc (sizeof(char*) * (strlen(buffer) - 1));
            strncpy(file_name, buffer, strlen(buffer) - 1);

            buffer_send = (char *) malloc(sizeof(buffer_send) * strlen(buffer) + 10);

            /*Format the input string */
            strcpy(buffer_send, "FILE\n");
            strcat(buffer_send, buffer);
            printf("before port is attached: %s", buffer_send);
            
            strcat(buffer_send, tcpPort);
            strcat(buffer_send, "\n");

            /*Debug*/
            printf("to be send: %s\n", buffer_send);
            printf("to be send len: %d\n", strlen(buffer_send));

            /* Send message to server. */
            if (sendto(socket_udp, buffer_send, strlen(buffer_send), 0, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
                perror("Send to, failed.");
                exit(EXIT_FAILURE);
            }

            /*[> Read message from server. <]*/
            /*read(socket_tcp, buffer_received, MAX_LINE-1);*/

            /*[> write the data to the file. <]*/
            /*if (strncmp(buffer_received + 2, "NOT FOUND", 9) == 0) {*/
            /*printf("Server responded: %s\n", buffer_received + 2);*/
            /*}*/
            /*else {*/
            /*fp = fopen(file_name, "wb");*/
            /*fwrite(buffer_received, 1, strlen(buffer_received), fp);*/
            /*printf("Server responded: Data is written to the file named: %s\n", file_name);*/
            /*[> close the file and free the memory <]*/
            /*fclose(fp);*/

            /*Free memory*/
            free(file_name);
            free(buffer);
            free(buffer_send);
        }
            else if (strncmp(buffer, "q", 1) == 0) {
                fprintf(stderr, "Now should exit.\n");
                if (close(socket_tcp) < 0 ) {
                    fprintf(stderr, "ECHOSERV: Error calling close()\n");
                    exit(EXIT_FAILURE);
                }

                return EXIT_SUCCESS;
            }
            else 
                printf("\nInvalid Command: Press 's' for echo, 't' for file storage and 'q' for exit.\n");
        } while (strncmp(buffer, "q", 1) != 0);

        return EXIT_SUCCESS;
    }


    int ParseCmdLine(int argc, char *argv[], char **szAddress, char **tcpPort, char **udpPort) {
        /* Number of arguments: <client> <TCP port> <server IP> <server UDP Port> (4 arguments). */
        if (argc == 4) {
            *tcpPort = argv[1];
            *szAddress = argv[2];
            *udpPort = argv[3];
        }
        else {
            printf("Usage:\n\n");
            printf("    <client> <TCP port> <server IP> <server UDP Port>\n\n");
            exit(EXIT_SUCCESS);
        }
        return 0;
    }

