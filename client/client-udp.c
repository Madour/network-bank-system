#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>

#include "common/utils.h"


int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Command usage : ./client <port_number>\n");
        return EXIT_SUCCESS;
    }
    if (!isUInt(argv[1])) {
        fprintf(stderr, "Error : Argument <port_number> must be an unsigned int.\n");
        return EXIT_FAILURE;
    }
    int port_number = atoi(argv[1]);

    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        fprintf(stderr, "Error : failed to create socket\n");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port_number);
    
    // communication with server
    char cmd_buffer[255];
    char response_buffer[1024];
    int exit = 0;
    unsigned int len;
    int msg_size;
    while(1) {
        // reset buffer
        bzero(cmd_buffer, sizeof(cmd_buffer));

        // read user input
        int n = 0;
        printf("> ");
        while ( (cmd_buffer[n++] = getchar()) != '\n');
        cmd_buffer[n-1] = '\0';
        
        exit = (strcmp(cmd_buffer, "exit") == 0);

        // send to server
        msg_size = sendto(socket_fd, cmd_buffer, strlen(cmd_buffer), MSG_CONFIRM, (struct sockaddr*)&server_addr, sizeof(server_addr));
        if (msg_size <= 0) {
            printf("[INFO] Failed to send message to server %s:%u. Please retry.\n\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
            continue;
        }

        // reset buffer
        bzero(response_buffer, sizeof(response_buffer));
        // read server response
        msg_size = recvfrom(socket_fd, response_buffer, sizeof(response_buffer), MSG_WAITALL, (struct sockaddr*)&server_addr, &len);
        printf("(%s)\n\n", response_buffer);
        if (msg_size < 0)
            printf("[INFO] Failed to read response from server %s:%u (received %d bytes)\n\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port), msg_size);

        if (exit == 1) {
            printf("[INFO] Closing connection\n");
            break;
        }

    }

    close(socket_fd);
    return 0;
}
