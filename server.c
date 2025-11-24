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

int parse_query(char* query, const int query_len);
int parse_request(char* request, const int query_len);
int parse_formula(char* formula, const int formula_len);

int main(int argc, char *argv[]) {
    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        fatal("Could not open socket");
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        fatal("Could not bind");
    }

    if (listen(socket_fd, LISTEN_BACKLOG) == -1) {
        fatal("Could not listen");
    }

    struct sockaddr_in peer_addr;
    socklen_t peer_addr_len = sizeof(peer_addr);
    int conn_fd = accept(socket_fd, (struct sockaddr*)&peer_addr, &peer_addr_len);
    if (conn_fd == -1) {
        fatal("Could not accept");
    }

    char buffer[BUFFERSIZE];
    int n = read(conn_fd, buffer, sizeof(buffer));
    if (n == -1) {
        fatal("Could not read");
    }
    
    int result = parse_query(buffer, n);
    char response[256];
    sprintf(response, "aa: %d", result);
    if (write(conn_fd, response, strlen(response)) == -1) {
        fatal("Could not send");
    }

    close(conn_fd);
    close(socket_fd);
    
    return 0;
}

int parse_query(char* query, const int query_len) {
    char method[16], request[256], version[16];
    char *token;
    token = strtok(query, " ");
    if (token == NULL) {
        fatal("HTTP header error");
    }
    strcpy(method, token);

    token = strtok(NULL, " ");
    if (token == NULL) {
        fatal("HTTP header error");
    }
    strcpy(request, token);

    token = strtok(NULL, " ");
    if (token == NULL) {
        fatal("HTTP header error");
    }
    strcpy(version, token);

    if (strtok(NULL, " ") != NULL) {
        fatal("HTTP header error");
    }

    if (strcmp(method, "GET") != 0) {
        fatal("only HTTP GET is supported.");
    }

    if (strcmp(version, "HTTP/1.1") != 0) {
        fatal("Only HTTP/1.1 is supported");
    }
    
    return parse_request(request, strlen(request));
}

int parse_request(char* request, const int request_len) {
    const char request_prefix[] = "/calc?query=";
    if (strncmp(request, request_prefix, strlen(request_prefix)) != 0) {
        fatal("Invalid format");
    }
    char formula[256];
    strcpy(formula, request + strlen(request_prefix));
    return parse_formula(formula, strlen(formula));
}

int parse_formula(char* formula, const int formula_len) {
    int operator_position = -1;
    for (int i = 0; i < formula_len; ++i) {
        if (formula[i] == '+' || formula[i] == '-') {
            operator_position = i;
            break;
        }
    }
    if (operator_position == -1) {
        return atoi(formula);
    }

    char lhs[256], rhs[256];
    strncpy(lhs, formula, operator_position);
    strcpy(rhs, formula + operator_position + 1);

    if (formula[operator_position] == '+') {
        return atoi(lhs) + parse_formula(rhs, formula_len - operator_position - 1);
    }
    if (formula[operator_position] == '-') {
        return atoi(lhs) + parse_formula(rhs, formula_len - operator_position - 1);
    }

    perror("Invalid formula format");
    exit(EXIT_FAILURE);
}