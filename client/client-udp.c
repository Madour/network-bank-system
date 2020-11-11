#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Command usage : ./client <port_number>\n");
        return EXIT_SUCCESS;
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
        sendto(socket_fd, cmd_buffer, strlen(cmd_buffer), MSG_CONFIRM, (struct sockaddr*)&server_addr, sizeof(server_addr));

        // reset buffer
        bzero(response_buffer, sizeof(response_buffer));
        // read server response
        int read_size = recvfrom(socket_fd, response_buffer, sizeof(response_buffer), MSG_WAITALL, (struct sockaddr*)&server_addr, &len);
        if (read_size <= 0)
            printf("[INFO] Failed to read response from server %s:%u\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
        else
            printf("(%s)\n\n", response_buffer);

        if (exit == 1) {
            printf("[INFO] Closing connection\n");
            break;
        }

    }

    close(socket_fd);
    return 0;
}
