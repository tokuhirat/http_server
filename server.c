#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "util.h"

#define LISTEN_BACKLOG 50
#define BUFFERSIZE 1024

int parse_query(char* query, const int query_len, int *result);
int parse_request(char* request, const int query_len, int *result);
int parse_formula(char* formula, const int formula_len, int *result);
int get_num_digits(int value);

int main(int argc, char *argv[]) {
    if (argc > 2) {
        fatal("argument error");
    }

    int port = 8080;
    if (argc == 2) {
        port = atoi(argv[1]);
    }

    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        fatal("Could not open socket");
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        fatal("Could not bind");
    }

    if (listen(socket_fd, LISTEN_BACKLOG) == -1) {
        fatal("Could not listen");
    }

    while (1) {
        struct sockaddr_in *peer_addr = NULL;
        char *buffer = NULL;
        char *response = NULL;

        socklen_t peer_addr_len = sizeof(struct sockaddr_in);
        peer_addr = calloc(1, peer_addr_len);
        if (peer_addr == NULL) {
            perror("calloc error");
            goto cleanup;
        }
        int conn_fd = accept(socket_fd, (struct sockaddr*)peer_addr, &peer_addr_len);
        if (conn_fd == -1) {
            perror("Could not accept");
            goto cleanup;
        }

        buffer = calloc(BUFFERSIZE, sizeof(char));
        if (buffer == NULL) {
            perror("calloc failed");
            goto cleanup;
        }
        int n = read(conn_fd, buffer, BUFFERSIZE);
        if (n == -1) {
            perror("Could not read");
            goto cleanup;
        }

        int result;
        if (parse_query(buffer, n, &result) == -1) {
            perror("Could not read");
            goto cleanup;
        }

        response = calloc(BUFFERSIZE, sizeof(char));
        if (response == NULL) {
            perror("calloc failed");
            goto cleanup;
        }
        sprintf(response, "HTTP/1.1 200 OK\r\nContent-Length:%d\r\n\r\n%d\r\n", get_num_digits(result), result);
        if (write(conn_fd, response, strlen(response)) == -1) {
            perror("Could not send");
            goto cleanup;
        }

cleanup:
        free(peer_addr);
        free(buffer);
        free(response);
        close(conn_fd);
    }

    close(socket_fd);
    
    return 0;
}

int parse_query(char* query, const int query_len, int *result) {
    char method[16], request[256], version[16];
    char *token;
    token = strtok(query, " ");
    if (token == NULL) {
        perror("HTTP header error");
        return -1;
    }
    strcpy(method, token);

    token = strtok(NULL, " ");
    if (token == NULL) {
        perror("HTTP header error");
        return -1;
    }
    strcpy(request, token);

    token = strtok(NULL, " ");
    if (token == NULL) {
        perror("HTTP header error");
        return -1;
    }
    strcpy(version, token);

    if (strtok(NULL, " ") != NULL) {
        perror("HTTP header error");
        return -1;
    }

    if (strcmp(method, "GET") != 0) {
        perror("only HTTP GET is supported.");
        return -1;
    }

    if (strcmp(version, "HTTP/1.1") != 0) {
        perror("Only HTTP/1.1 is supported");
        return -1;
    }
    
    return parse_request(request, strlen(request), result);
}

int parse_request(char* request, const int request_len, int *result) {
    const char request_prefix[] = "/calc?query=";
    if (strncmp(request, request_prefix, strlen(request_prefix)) != 0) {
        perror("Invalid format");
        return -1;
    }
    char formula[256];
    strcpy(formula, request + strlen(request_prefix));
    return parse_formula(formula, strlen(formula), result);
}

int parse_formula(char* formula, const int formula_len, int *result) {
    char *p = formula;
    int value = strtol(p, &p, 10);
    while (*p) {
        if (*p == '+') {
            ++p;
            if (!*p) {
                perror("Invalid formula format");
                return -1;
            }
            value += strtol(p, &p, 10);
            continue;
        }
        if (*p == '-') {
            ++p;
            if (!*p) {
                perror("Invalid formula format");
                return -1;
            }
            value -= strtol(p, &p, 10);
            continue;
        }
        perror("Invalid formula format");
        return -1;
    }
    *result = value;
    return 0;
}

int get_num_digits(int value) {
    int num_digits = 0;
    while (value) {
        ++num_digits;
        value /= 10;
    }
    return num_digits;
}
