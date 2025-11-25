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
    if (argc != 1 && argc != 3) {
        fatal("argument error");
    }
    
    char IP_addr[] = "127.0.0.1";
    int port = 8080;
    if (argc == 3) {
        strcpy(IP_addr, argv[1]);
        port = atoi(argv[2]);
    }

    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        fatal("Could not open socket");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

	if (inet_aton(IP_addr, &server_addr.sin_addr) == 0) {
		fatal("Invalid IP Address");
	}

    if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        fatal("Could not connect");
    }

    char formula[256] = {0};
    scanf("%s", formula);
    char query[256];
    sprintf(query, "GET /calc?query=%s HTTP/1.1", formula);

    if (write(socket_fd, query, strlen(query)) == -1) {
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