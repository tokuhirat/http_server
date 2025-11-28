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
    char *formula = NULL;
    char *query = NULL;
    char *buffer = NULL;
    char IP_addr[] = "127.0.0.1";
    int port = 8080;

    if (argc != 1 && argc != 3) {
        perror("argument error");
        goto cleanup;
    }
    
    if (argc == 3) {
        strcpy(IP_addr, argv[1]);
        port = atoi(argv[2]);
    }

    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("Could not open socket");
        goto cleanup;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_aton(IP_addr, &server_addr.sin_addr) == 0) {
        perror("Invalid IP Address");
        goto cleanup;
    }

    if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Could not connect");
        goto cleanup;
    }

    formula = calloc(256, sizeof(char));
    if (formula == NULL) {
        perror("calloc failed");
        goto cleanup;
    }
    scanf("%s", formula);

    query = calloc(256, sizeof(char));
    if (query == NULL) {
        perror("calloc failed");
        goto cleanup;
    }
    sprintf(query, "GET /calc?query=%s HTTP/1.1", formula);

    if (write(socket_fd, query, strlen(query)) == -1) {
        perror("GET failed");
        goto cleanup;
    }

    buffer = calloc(BUFFERSIZE, sizeof(char));
    if (buffer == NULL) {
        perror("calloc failed");
        goto cleanup;
    }

    int n = read(socket_fd, buffer, BUFFERSIZE);
    if (n == -1) {
        perror("Could not read");
        goto cleanup;
    }

    if (write(1, buffer, strlen(buffer)) == -1) {
        perror("write to stdout failed");
        goto cleanup;
    }

cleanup:
    free(formula);
    free(query);
    free(buffer);

    if (socket_fd) {
        close(socket_fd);
    }
}
