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
    if (argc < 3) {
        printf("Command usage : ./client <host_name> <port_number>\n");
        return EXIT_SUCCESS;
    }
    char* host_name = argv[1];
    if (!isUInt(argv[2])) {
        fprintf(stderr, "Error : Argument <port_number> must be an unsigned int.\n");
        return EXIT_FAILURE;
    }
    int port_number = atoi(argv[2]);

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        fprintf(stderr, "Error : failed to create socket\n");
        return EXIT_FAILURE;
    }

    struct hostent* host_info = gethostbyname(host_name);
    if (host_info == NULL) {
        fprintf(stderr, "Error : cannot get host by name %s.\n", host_name);
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_addr = *(struct in_addr *)host_info->h_addr_list[0];
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_number);
    
    if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))) {
        fprintf(stderr, "Error : connection with the server failed.\n");
        return 1;
    }
    printf("[INFO] Connected to server %s:%u \n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));

    // communication with server
    char cmd_buffer[255];
    char response_buffer[1024];
    int close_connection = 0;
    while(1) {
        // reset buffer
        bzero(cmd_buffer, sizeof(cmd_buffer));

        // read user input
        int n = 0;
        printf("> ");
        while ( (cmd_buffer[n++] = getchar()) != '\n');
        cmd_buffer[n-1] = '\0';
        
        if (strcmp(cmd_buffer, "exit") == 0) {
            close_connection = 1;
        }

        // send to server
        write(socket_fd, cmd_buffer, sizeof(cmd_buffer));

        // reset buffer
        bzero(response_buffer, sizeof(response_buffer));
        // read server response
        int read_size = read(socket_fd, response_buffer, sizeof(response_buffer));
        if (read_size <= 0) {
            printf("[INFO] Failed to read from server %s:%u\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
            printf("[INFO] Closing connection.\n\n");
            break;
        }
        printf("(%s)\n\n", response_buffer);

        if (close_connection == 1) {
            printf("[INFO] Closing connection\n");
            break;
        }

    }

    close(socket_fd);
    return 0;
}
