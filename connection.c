
int main() {
    int sockfd;
    struct sockaddr_in server_addr;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0); //IPV4, TCP, 0 for default protocol (OS picks)
    
    //If socket fails show error from OS (perror) and exit with error code 1
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    

}