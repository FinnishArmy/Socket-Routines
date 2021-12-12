#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<time.h>
#include<string.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<errno.h>
#include<unistd.h>
#define CLI_PORT 49999
#define MAX 100
#define LISTENBUFFER 10


extern int errno;
//* README *//
// Usage:
// Compile: gcc -o {NAME} assignment8.c
// 1.] Create a server to listen with: ./{NAME} server
// 2.] Create a client to request with: ./{NAME} client 127.0.0.1
void checkHostName(int hostname) {
  if (hostname == -1) {
    perror("Error");
    exit(1);
  }
}

void checkHostEntry(struct hostent * hostentry) {
  if (hostentry == NULL) {
    perror("Error");
    exit(1);
  }
}

// int valueinarray(char val, char *iparray[]) {
//   int i;
//   for (i = 0; i < sizeof(*iparray[]); i++) {
//     if (*iparray[i] == val) return 1;
//   }
//   return 0;
// }

int hostname_to_ip(char * hostname, char * ip) {
  struct hostent *he;
  struct in_addr **addr_list;
  int i;
  if ((he = gethostbyname(hostname)) == NULL) {
    return 1;
  }
  addr_list = (struct in_addr **) he->h_addr_list;

  for (i=0; addr_list[i] != NULL; i++) {
    strcpy(ip, inet_ntoa(*addr_list[i]));
    return 0;
  }
  return 1;
}

int main(int argc, char **argv) {
    int clientID = 0;
    char error[] = "Name or service not known";
    //* Client Connection *//
    if (strcmp(argv[1], "client") == 0){
      if(argc != 3){
        printf("Enter a IP or HostName!");
        exit(0);
      }
      int sockfd;
      //* Make a socket *//
      if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // If result is < 0, there is an error
        perror("Error");
        exit(0);
      }
      char response[20];


      //* Bind the socket to a port *//
      //* Getting a text host name *//
      struct sockaddr_in servAddr; // Create a struct for the IP
      // char str[INET_ADDRSTRLEN]; // Setup a char array to convert the hostname to an IP
      // inet_pton(AF_INET, argv[2], &(servAddr.sin_addr));
      // inet_ntop(AF_INET, &(servAddr.sin_addr), str, INET_ADDRSTRLEN); // Use the str which will now be an IP

      char *hostname = argv[2];
      char ip[100];
      hostname_to_ip(hostname, ip);

      servAddr.sin_family = AF_INET;
      servAddr.sin_addr.s_addr = inet_addr(ip); // Set the IP to the command line arg.
      servAddr.sin_port = htons(CLI_PORT); // Set the IP to 49999 as per write-up.



      //* Connect to the server *//
      int s = connect(sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr));
      if (s != 0) {
        printf("Error: %s\n", error);
        // herror("4Error"); // If there is an aerror, the result is < 0.
        exit(0);
      } // The client's address is written into the sockaddr structure.

      if (recv(sockfd, response, 19, 0) < 0) {
        perror("Error");
        exit(0);
      }
      response[strlen(response)-1] = '\0';
      printf("%s\n", response);
      return 0;
    }

    //* Server Runner *//
    if (strcmp(argv[1], "server") == 0) {
      if(argc != 2){
        exit(0);
      }
      int listenfd;
      //* Create socket *//
      if (((listenfd) = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error");
        exit(0);
      } else {
      struct sockaddr_in servAddr;
      memset(&servAddr, '\0', sizeof(servAddr));
      //* Prepare the sockaddr structure *//
      servAddr.sin_family = AF_INET;
      servAddr.sin_port = htons(CLI_PORT);
      servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
      //* Bind the socket to a port *//
      int f = bind( listenfd,
              (struct sockaddr *) &servAddr, sizeof(servAddr));
      if (f != 0) {
        perror("Error"); // If the server is already open, print so
        exit(0);
      }
      }

      //* Listen and Accept connection *//
      if (listen(listenfd, 1) < 0) { // Sets up a connection queue one level deep
        perror("Error"); // If result isn't 0, an error is the result
        exit(0);
      }

      while(1){
        int client_socket = accept(listenfd, NULL, NULL);
        if (client_socket < 0) {
          perror("Error");
          exit(0);
        }
        clientID++;
          while(1) {
            time_t client_time;
            time(&client_time);
            send(client_socket, ctime(&client_time), 30, 0);
            if (send(client_socket, ctime(&client_time), 30, 0) < 0) {
              perror("Error");
              exit(0);
            }
            char hostBuffer[256];
            int hostname = gethostname(hostBuffer, sizeof(hostBuffer));
            if (hostname < 0) {
              perror("Error");
              exit(0);
            }
            checkHostName(hostname);

            printf("%s %d\n", hostBuffer, clientID);
            break;

          }
        }
        close(listenfd);
      return 0;
    }

    //* No command line arguments were given, so stop the code *//
    else {
      printf("No argument provided.");
      exit(0);
    }
return 0;
}
