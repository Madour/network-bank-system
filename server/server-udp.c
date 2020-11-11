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

#include "db.h"
#include "cli.h"
#include "common/utils.h"


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
    if (!isUInt(argv[1])) {
        fprintf(stderr, "Error : Argument <port_number> must be an unsigned int.\n");
        return EXIT_FAILURE;
    }
    int port_number = atoi(argv[1]);

    // will call exit_server when SIGINT is received (Ctrl+C)
    struct sigaction custom_sigint;
    bzero(&custom_sigint, sizeof(custom_sigint));
    custom_sigint.sa_handler = &exit_server;
    sigaction(SIGINT, &custom_sigint, &default_sigint);
    
    bank = DB_CreateBank();

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
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

    struct sockaddr_in client_addr;
    unsigned int len = sizeof(client_addr);
    int msg_size;
    while (1) {
        
        char cmd_buffer[255];
        int exit = 0;
        // reset buffer
        bzero(cmd_buffer, sizeof(cmd_buffer));

        // reset client addr
        bzero(&client_addr, sizeof(client_addr));
        // read client message
        msg_size = recvfrom(socket_fd, cmd_buffer, sizeof(cmd_buffer), MSG_WAITFORONE, (struct sockaddr*)&client_addr, &len);
        if (msg_size <= 0) {
            printf("[INFO] Failed to read message from client %s:%u\n\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            continue;
        }
        printf("Client %s:%u :\n(%s)\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), cmd_buffer);

        // split string received and store words in args array
        char* args[150];
        int argn = splitString(" ", cmd_buffer, args);

        char response[1024];
            
        exit = CLI_ExecCmd(argn, args, response, bank);
                
        // respond to client
        msg_size = sendto(socket_fd, response, strlen(response), MSG_CONFIRM, (struct sockaddr*)&client_addr, len);
        printf("> %s\n\n", response);
        if (msg_size <= 0) {
            printf("[INFO] Failed to send response to client %s:%u\n\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        }
        if (exit == 1) {
            printf("[INFO] Client %s:%u exited\n\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        }
        
    }

    DB_FreeBank(bank);

    close(socket_fd);

    return 0;
}
