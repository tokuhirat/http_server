#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("Could not open socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);

	if (inet_aton("127.0.0.1", &server_addr.sin_addr) == 0) {
		perror("Invalid IP Address");
		exit(EXIT_FAILURE);
	}

    if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Could not connect");
        exit(EXIT_FAILURE);
    }

    char s[] = "test data";
    write(socket_fd, s, sizeof(s));
    close(socket_fd);
}