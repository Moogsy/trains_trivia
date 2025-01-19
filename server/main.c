#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

#define BUFFER_SIZE 1024
#define DB_PATH "data/trains.db"
#define PORT 8080
#define MAX_CONNECTIONS 5

int handle_client(int client_sock);

int main(void) {

    // Db stuff, might move it somewhere else
    sqlite3 *db;

    if (sqlite3_open(DB_PATH, &db) != SQLITE_OK) {
        fprintf(
            stderr, 
            "Cannot open database: %s\n",
            sqlite3_errmsg(db)
        );
        return EXIT_FAILURE;
    }

    // Network, this block is stupidly big, I hate it
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr = {
        .sin_family=AF_INET,
        .sin_addr.s_addr=INADDR_ANY,
        .sin_port=htons(PORT)
    };

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_sock);
        return EXIT_FAILURE;
    }

    if (listen(server_sock, MAX_CONNECTIONS) < 0) {
        perror("listen");
        close(server_sock);
        return EXIT_FAILURE;
    }

    printf("Server listening on port %d.\n", PORT);

    // Big aah loop
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_len);

        if (client_sock < 0) {
            perror("accept");
            continue;
        }

        printf("Accepting a new connection.\n");

        // Joys of forking
        // But I'm tired, too bad !
        pid_t pid = fork();

        // Errored out, make sure to clean up everything
        if (pid < 0) {
            perror("fork");
            close(client_sock);
        } 
        // Child process
        // What do you mean this is cursed indent ?
        else if (pid == 0) {
            return handle_client(client_sock);
        }

        // Parent process
        int child_status;
        waitpid(pid, &child_status, 0);
        close(client_sock);
    }

    close(server_sock);
    sqlite3_close(db);
    return EXIT_SUCCESS;
}

int handle_client(int client_sock) {
    for (int i = 0; i < 3; i++) {
        // Receive message from client
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);
        if (recv(client_sock, buffer, BUFFER_SIZE, 0) <= 0) {
            perror("recv");
            close(client_sock);
            exit(EXIT_FAILURE);
        }
        printf("Client: %s\n", buffer);

        // Send response to client
        snprintf(buffer, BUFFER_SIZE, "Message %d from server", i + 1);
        if (send(client_sock, buffer, strlen(buffer), 0) == -1) {
            perror("send");
            close(client_sock);
            exit(EXIT_FAILURE);
        }
    }
    close(client_sock);
    exit(EXIT_SUCCESS);
}
