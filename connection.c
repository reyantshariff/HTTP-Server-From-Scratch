#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {
    int sockfd;
    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0); //IPV4, TCP, 0 for default protocol (OS picks)
    
    //If socket fails show error from OS (perror) and exit with error code 1
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    // Set up server address structure
    struct sockaddr_in server_addr={
        .sin_family = AF_INET, //IPV4
        .sin_port = htons(8080), //Port number (convert to network byte order)
        .sin_addr.s_addr = INADDR_ANY,
    };

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        return 1;
    }
    int backlog=10; //Number of pending connections queue will hold
    if (listen(sockfd, backlog) < 0) {
        perror("listen");
        return 1;
    }
    // Accept a connection (blocking call)
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);

    if (clientfd < 0) {
        perror("accept");
        return 1;
    }
}