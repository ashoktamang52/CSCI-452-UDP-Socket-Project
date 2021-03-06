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
#define LISTENQ            (1024)  /* Backlog for listen() */


/*  Function declarations  */

int ParseCmdLine(int argc, char *argv[], char **szAddress, char **tcpPort, char **udpPort);


/*  main()  */

int main(int argc, char *argv[]) {

    int       socket_tcp;                   /*  connection socket                   */
    int	      socket_udp;
    int       list_s;                       /* listening tcp socket                 */
    short int tcp_port;                     /*  tcp port number                     */
    short int udp_port;                     /*  udp port number                     */
    struct    sockaddr_in servaddr_udp;     /*  socket address structure            */
    struct    sockaddr_in servaddr_tcp;     /*  tcp socket address structre         */
    struct    sockaddr_in remaddr;          /*  remote address               	    */
    socklen_t addrlen = sizeof(remaddr);    /*  length of remote address            */
    char     *buffer;                       /*  character buffer                    */
    char     *buffer_send;                  /*  Holds message to be send to server  */
    char     *buffer_received;              /*  Holds message send by server        */
    char     *szAddress;                    /*  Holds remote IP address             */
    char     *udpPort;                      /* Holds server upd port                */
    char     *tcpPort;                      /*  Holds server tcp port               */
    char     *endptr;                       /*  for strtol()                        */
    FILE     *fp;                           /*  file pointer                        */
    char     *file_name;                    /*  for creating and writing data into  */
    char     *temp;                         /*  temp storage/formatting/printing    */
    
    int       sendstatus;
    int       recvlen;


    /*  Get command line arguments  */

    ParseCmdLine(argc, argv, &szAddress, &tcpPort, &udpPort);

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

    memset(&servaddr_udp, 0, sizeof(servaddr_udp));
    servaddr_udp.sin_family      = AF_INET;
    servaddr_udp.sin_port        = htons(udp_port);
    servaddr_udp.sin_addr.s_addr = htonl(INADDR_ANY);

    /*  Get string to follow user commands */
    int query_count = 0;
    while (1) {
        buffer = (char *) malloc(sizeof(buffer) * MAX_LINE);
        printf("Insert your command: ");
        fgets(buffer, MAX_LINE, stdin);
        
        /* The length of the command should be 2 to account for null character. */
        if (strncmp(buffer, "s", 1) == 0 && strlen(buffer) == 2) {
            /*Reset buffer to get the string.*/
            free(buffer);
            buffer = (char *) malloc(sizeof(buffer) * MAX_LINE);
            
            printf("\nPlease Enter a string: ");
            fgets(buffer, MAX_LINE, stdin); /* fgets also reads new line character. */
            
            /* Allocate a new buffer to send message to server. */
            buffer_send =  (char *) malloc(sizeof(buffer_send) * (strlen(buffer)));
            
            /* Format the input string */
            strcpy(buffer_send, "CAP\n");
            strcat(buffer_send, buffer);
            
            /*Send message to server via UDP*/
            sendstatus = 0;
            sendstatus = sendto(socket_udp, buffer_send, strlen(buffer_send), 0, (struct sockaddr *) &servaddr_udp, sizeof (servaddr_udp));
            if (sendstatus < 0) {
                perror("Send to, failed.");
                exit(EXIT_FAILURE);
            }
            
            /*Read message back from server via UDP*/
            recvlen = 0;
            buffer_received = (char *) malloc(sizeof(buffer_received) * MAX_LINE);
            recvlen = recvfrom(socket_udp, buffer_received, MAX_LINE, 0, (struct sockaddr *) &remaddr, &addrlen);
            if (recvlen > 0) {
                buffer_received[recvlen] = '\0';
            }
            
            printf("Server responded: %s\n", buffer_received);
            
            /* reset buffer to get only relevant string */
            free(buffer_received);
            free(buffer_send);
        }
        else if (strncmp(buffer, "t", 1) == 0 && strlen(buffer) == 2) {
            /*Reset buffer to get the string.*/
            free(buffer);
            buffer = (char *) malloc(sizeof(buffer) * MAX_LINE);
            
            printf("\nPlease Enter a filename to search: ");
            fgets(buffer, MAX_LINE, stdin);
            
            file_name = (char*) malloc (sizeof(char*) * (strlen(buffer) - 1));
            strncpy(file_name, buffer, strlen(buffer) - 1);
            
            buffer_send = (char *) malloc(sizeof(buffer_send) * strlen(buffer) + 10);
            
            /*Format the input string */
            strcpy(buffer_send, "FILE\n");
            strcat(buffer_send, buffer);
            
            strcat(buffer_send, tcpPort);
            strcat(buffer_send, "\n");
            
            
            /* Send message to server. */
            sendstatus = 0;
            sendstatus = sendto(socket_udp, buffer_send, strlen(buffer_send), 0, (struct sockaddr *) &servaddr_udp, sizeof(servaddr_udp));
            if ( sendstatus < 0) {
                perror("Send to, failed.");
                exit(EXIT_FAILURE);
            }
            
            /*Recieve status from server.*/
            recvlen = 0;
            buffer_received = (char *) malloc(sizeof(buffer_received) * MAX_LINE);
            recvlen = recvfrom(socket_udp, buffer_received, MAX_LINE, 0, (struct sockaddr *) &remaddr, &addrlen);
            if (recvlen > 0) {
                buffer_received[recvlen] = '\0';
            }
            
            /*Check status of file from server.*/
            char *status_token = (char *) malloc(sizeof(status_token) * MAX_LINE);
            strcpy(status_token, buffer_received);
            status_token = strtok(status_token, "\n");
            
            /*Holds file name without new line character.*/
            temp = (char *) malloc(sizeof(temp) * MAX_LINE);
            strncpy(temp, buffer, strlen(buffer) - 1);
            
            /*If file does exist.*/
            if (strcmp(status_token, "OK") == 0) {
                /* Get the size of the file to be transferred. */
                char *fileSize;
                fileSize = (char *) malloc(sizeof(char*) * MAX_LINE);
                
                int position = 0;
                while(status_token != NULL) {
                    if (position == 1)
                        strcpy(fileSize, status_token);
                    if (position > 1)
                        break;
                    position++;
                    status_token = strtok(NULL, "\n");
                }
                
                /*Set up TCP connection*/
                
                tcp_port = strtol(tcpPort, &endptr, 0);
                if ( *endptr ) {
                    fprintf(stderr, "ECHOSERV: Invalid port number.\n");
                    exit(EXIT_FAILURE);
                }
                
                /*  Create the listening socket  */
                
                if ((list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
                    fprintf(stderr, "ECHOCLNT: Error creating listening socket.\n");
                    exit(EXIT_FAILURE);
                }
                
                /*Set all bytes in socket address structure to*/
                /*zero, and fill in the relevant data members   */
                
                memset(&servaddr_tcp, 0, sizeof(servaddr_tcp));
                servaddr_tcp.sin_family      = AF_INET;
                servaddr_tcp.sin_port        = htons(tcp_port);
                servaddr_tcp.sin_addr.s_addr = htonl(INADDR_ANY);
                
                if (bind(list_s, (struct sockaddr *) &servaddr_tcp, sizeof(servaddr_tcp)) < 0) {
                    perror("Error binding listening socket:");
                    exit(0);
                }
                
                if (listen(list_s, LISTENQ) < 0) {
                    perror("Error listening to socket");
                    exit(0);
                }
                
                while (1) {
                    if ((socket_tcp = accept(list_s, NULL, NULL)) < 0) {
                        perror("Error connecting TCP server:");
                    }
                    
                    memset(&endptr, 0, sizeof(endptr));
                    int file_size = strtol(fileSize, &endptr, 0);
                    
                    /* Get file from the server. */
                    void *large_buffer;
                    large_buffer = (void *) malloc(sizeof(large_buffer) * file_size);
                    read(socket_tcp, large_buffer, file_size);
                    
                    /* write the data to the file. */
                    fp = fopen(file_name, "wb");
                    
                    int writelen;
                    if ((writelen = fwrite(large_buffer, 1, file_size, fp)) < 0) {
                        perror("Error writing");
                        exit(0);
                    }
                    
                    /* close the file and free the memory */
                    fclose(fp);
                    free(large_buffer);
                    
                    if (close(socket_tcp) < 0) {
                        perror("Error closing tcp connection");
                    }
                    
                    break;
                }
                
                if (close(list_s) < 0) {
                    perror("Error closing listening socket");
                }
                free(status_token);
                free(fileSize);

                
            }
            else {
                printf("%s not found.\n\n", temp);
            }
            
            /*Free memory*/
            free(buffer_received);
            free(buffer_send);
            free(file_name);
            free(temp);
        }
        else if (strncmp(buffer, "q", 1) == 0 && strlen(buffer) == 2) {
            fprintf(stderr, "Now exiting...\n");
            return EXIT_SUCCESS;
        }
        else 
            printf("\nInvalid Command: Press 's' for echo, 't' for file storage and 'q' for exit.\n");
    }

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

