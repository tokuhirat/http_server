#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define LISTEN_BACKLOG 50
#define BUFFERSIZE 1024

int main(int argc, char *argv[]) {
    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("Could not open socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("Could not bind");
        exit(EXIT_FAILURE);
    }

    if (listen(socket_fd, LISTEN_BACKLOG) == -1) {
        perror("Could not listen");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in peer_addr;
    socklen_t peer_addr_len = sizeof(peer_addr);
    int conn_fd = accept(socket_fd, (struct sockaddr*)&peer_addr, &peer_addr_len);
    if (conn_fd == -1) {
        perror("Could not accept");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFERSIZE];
    int n = read(conn_fd, buffer, sizeof(buffer));
    if (n == -1) {
        perror("Could not read");
        exit(EXIT_FAILURE);
    }
    write(1, buffer, n);

    close(conn_fd);
    close(socket_fd);
    
    return 0;
}
