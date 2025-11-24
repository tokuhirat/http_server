#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "util.h"

#define BUFFERSIZE 1024

int main(int argc, char *argv[]) {
    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        fatal("Could not open socket");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);

	if (inet_aton("127.0.0.1", &server_addr.sin_addr) == 0) {
		fatal("Invalid IP Address");
	}

    if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        fatal("Could not connect");
    }

    char s[] = "GET /calc?query=2+10 HTTP/1.1";
    if (write(socket_fd, s, sizeof(s)) == -1) {
        fatal("GET failed");
    }

    char buffer[BUFFERSIZE];
    int n = read(socket_fd, buffer, sizeof(buffer));
    if (n == -1) {
        fatal("Could not read");
    }

    if (write(1, buffer, strlen(buffer)) == -1) {
        fatal("write to stdout failed");
    }

    close(socket_fd);
}