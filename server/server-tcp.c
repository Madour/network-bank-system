#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <errno.h>
#include <limits.h>

#include "db.h"
#include "cli.h"

struct sigaction default_sigint;

static Bank* bank = NULL;
int socket_fd = -1;

void exit_server(int sig_no) {
    printf("\n[INFO] CTRL-C pressed (SIGINT). Closing server.\n");
    if (bank != NULL)
        DB_FreeBank(bank);
    if (socket_fd >= 0)
        close(socket_fd);
    sigaction(SIGINT, &default_sigint, NULL);
    kill(getpid(), SIGINT);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Commande usage : ./server <port_number> \n");
        return EXIT_SUCCESS;
    }
    int port_number = atoi(argv[1]);

    // will call exit_server when SIGINT is received (Ctrl+C)
    struct sigaction custom_sigint;
    bzero(&custom_sigint, sizeof(custom_sigint));
    custom_sigint.sa_handler = &exit_server;
    sigaction(SIGINT, &custom_sigint, &default_sigint);
    
    bank = DB_CreateBank();

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        fprintf(stderr, "Error : failed to create socket.\n");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port_number);

    if (bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))) {
        fprintf(stderr, "Error : socket bind failed.\n");
        return EXIT_FAILURE;
    }

    // queue up to 5 connections
    listen(socket_fd, 5);
    struct sockaddr_in client_addr;
    unsigned int len = sizeof(client_addr);

    while (1) {
        printf("[INFO] Server at %s listening on port %u\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));

        int connection_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &len);
        if (connection_fd < 0) {
            fprintf(stderr, "Error : server accept failed.\n");
            return EXIT_FAILURE;
        }
        printf("[INFO] Client %s:%u is now connected. \n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        
        char cmd_buffer[255];
        int close_connection = 0;
        while(1) {
            // reset buffer
            bzero(cmd_buffer, sizeof(cmd_buffer));

            // read client message
            int msg_size = read(connection_fd, cmd_buffer, sizeof(cmd_buffer));
            if (msg_size <= 0) {
                printf("[INFO] Failed to read from client %s:%u\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                printf("[INFO] Closing connection.\n\n");
                break;
            }
            printf("(%s)\n", cmd_buffer);

            // split string received and store words in args array
            int argn = 0;
            char* args[150];
            char *token, *rest = cmd_buffer;
            while( (token = strtok_r(rest, " ", &rest)) )
                args[argn++] = token;

            char response[1024];
            
            close_connection = CLI_ExecCmd(argn, args, response, bank);
                
            // respond to client
            write(connection_fd, response, strlen(response));
            printf("> %s\n\n", response);

            if (close_connection == 1) {
                printf("[INFO] Client %s:%u closed connection\n\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                break;
            }
        }
        
        close(connection_fd);
    }

    DB_FreeBank(bank);

    close(socket_fd);

    return 0;
}
