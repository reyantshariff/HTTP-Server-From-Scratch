#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void parse_request(char *raw, char *method, char *path); // forward declaration

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
    // Set socket options to allow reuse of address and port
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
    while (1) {
    // Accept a connection (blocking call)
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);

    if (clientfd < 0) {
        perror("accept");
        return 1;
    }
    char buffer[1024]; // Buffer to hold incoming request
    char method[16]; // Buffer to hold HTTP method (e.g., GET, POST)
    char path[256]; // Buffer to hold requested path (e.g., /index.html
    
    if (recv(clientfd, buffer, sizeof(buffer), 0) < 0) {
        perror("recv");
        return 1;
    }
    parse_request(buffer, method, path);
    printf("Method: %s, Path: %s\n", method, path);
    FILE *f = fopen("index.html", "r");
    if (f == NULL) {
    // file not found → send 404
    char response[] = "HTTP/1.1 404 Not Found\r\n\r\nFile Not Found";
    send(clientfd, response, strlen(response), 0);
    }
    else 
    {
        // file found → send 200 + file contents
        char response_header[] = "HTTP/1.1 200 OK\r\n\r\n";
        send(clientfd, response_header, strlen(response_header), 0);

        char file_buffer[1024];
        int bytes_read = fread(file_buffer, 1, sizeof(file_buffer), f);
        send(clientfd, file_buffer, bytes_read, 0);
    } 
    close(clientfd);
    }
    close(sockfd);
    return 0;
}
/**
 * Parses an HTTP request line to extract the method and path.
 *
 * @param raw    The raw HTTP request line (e.g., "GET /index.html HTTP/1.1").
 *               This string will be modified by the function.
 * @param method Buffer to store the extracted HTTP method (e.g., "GET").
 * @param path   Buffer to store the extracted request path (e.g., "/index.html").
 *
 * @note The function uses strtok to tokenize the input string and assumes
 *       that the method and path are separated by spaces. The input string
 *       'raw' will be modified during parsing.
 */
void parse_request(char *raw, char *method, char *path) 
{
    char *token = strtok(raw, " ");  // returns pointer to "GET"
    strcpy(method, token);            // copy it into your method buffer
    
    token = strtok(NULL, " ");       // returns pointer to "/index.html"
    strcpy(path, token);              // copy it into your path buffer
}
