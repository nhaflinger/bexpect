// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <string>
  
#define PORT     8080
#define MAXLINE 1024

using namespace std;
  
// Driver code
int main(int argc, char* argv[]) 
{
    int sockfd;
    char buffer[MAXLINE];
    char *hello = "Hello from client";
    struct sockaddr_in servaddr;
    
    /* read command line arguments */
    const char* program = argv[0];
    if (argc <= 1)
    {
        cout << "Host and port not defined" << endl;
        exit(0);
    }

    // parse command line
    string str1(argv[1]);
    string local_host_port;
    local_host_port.assign(str1);

    string delimiter = ":";
    string local_host, local_port;
    local_host = local_host_port.substr(0, local_host_port.find(delimiter));
    local_port = local_host_port.substr(local_host_port.find(delimiter)+1, local_host_port.back());
  
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
  
    memset(&servaddr, 0, sizeof(servaddr));
      
    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(local_port.c_str()));
    servaddr.sin_addr.s_addr = inet_addr(local_host.c_str());
      
    int n;
    socklen_t len;
      
    sendto(sockfd, (const char *)hello, strlen(hello), MSG_CONFIRM, (const struct sockaddr *) &servaddr,  sizeof(servaddr));
    printf("Hello message sent.\n");
          
    n = recvfrom(sockfd, (char *)buffer, MAXLINE,  MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
    buffer[n] = '\0';
    printf("Server : %s\n", buffer);
  
    close(sockfd);
    return 0;
}