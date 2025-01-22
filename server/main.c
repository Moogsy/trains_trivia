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
#include <time.h>
#include <signal.h>

#include "questions.h"
#include "types.h"
#include "yyjson.h"

#define BUFFER_SIZE 1024
#define NUM_CHOICES 4
#define DB_PATH "data/trains.db"
#define PORT 8080
#define MAX_CONNECTIONS 5

void setup_signal_handler();
void sigchld_handler(int signo);
sqlite3* setup_database();
int setup_server_socket();
void accept_client_connections(int server_sock, sqlite3* db);
int handle_client(int client_sock, sqlite3* db);
char* question_to_json(Question* q);

int main(void) {
    // Seed RNG
    srand(time(NULL));

    // Setup signal handling
    setup_signal_handler();

    // Setup database
    sqlite3* db = setup_database();
    if (!db) return EXIT_FAILURE;

    // Setup server socket
    int server_sock = setup_server_socket();
    if (server_sock < 0) {
        sqlite3_close(db);
        return EXIT_FAILURE;
    }

    // Accept client connections
    accept_client_connections(server_sock, db);

    // Cleanup
    close(server_sock);
    sqlite3_close(db);

    return EXIT_SUCCESS;
}

void setup_signal_handler() {
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);
}

void sigchld_handler(int signo) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

sqlite3* setup_database() {
    sqlite3* db;
    if (sqlite3_open(DB_PATH, &db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    // Enable WAL mode for better concurrency
    sqlite3_exec(db, "PRAGMA journal_mode=WAL;", NULL, NULL, NULL);
    return db;
}

int setup_server_socket() {
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(PORT)
    };

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_sock);
        return -1;
    }

    if (listen(server_sock, MAX_CONNECTIONS) < 0) {
        perror("listen");
        close(server_sock);
        return -1;
    }

    printf("Server listening on port %d.\n", PORT);
    return server_sock;
}

void accept_client_connections(int server_sock, sqlite3* db) {
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_len);

        if (client_sock < 0) {
            perror("accept");
            continue;
        }

        printf("Accepted a new connection.\n");

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            close(client_sock);
            continue;
        }

        if (pid == 0) { // Child process
            close(server_sock); // Child doesn't need the listening socket
            int status = handle_client(client_sock, db);
            sqlite3_close(db);
            exit(status);
        }

        // Parent process
        close(client_sock); // Parent doesn't need the client socket
    }
}

int handle_client(int client_sock, sqlite3* db) {
    srand(time(NULL) ^ getpid()); // Reseed RNG for the child process

    while (1) {
        char buffer_useless[BUFFER_SIZE] = {0};
        if (recv(client_sock, buffer_useless, BUFFER_SIZE, 0) <= 0) {
            perror("recv");
            close(client_sock);
            return EXIT_FAILURE;
        }

        Question question = generate_question(db);
        char* question_json = question_to_json(&question);

        if (!question_json) {
            fprintf(stderr, "Failed to generate JSON for question.\n");
            close(client_sock);
            return EXIT_FAILURE;
        }

        char buffer[BUFFER_SIZE];
        snprintf(buffer, BUFFER_SIZE, "%s", question_json);
        free(question_json); // Free allocated JSON memory

        if (send(client_sock, buffer, strlen(buffer), 0) == -1) {
            perror("send");
            close(client_sock);
            return EXIT_FAILURE;
        }
    }

    close(client_sock);
    return EXIT_SUCCESS;
}

char* question_to_json(Question* q) {
    yyjson_mut_doc* doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_val* root = yyjson_mut_obj(doc);
    yyjson_mut_doc_set_root(doc, root);

    yyjson_mut_obj_add_str(doc, root, "question", q->prompt);
    yyjson_mut_obj_add_int(doc, root, "correct_answer", q->correct_answer);

    yyjson_mut_val* choices = yyjson_mut_arr(doc);
    for (size_t i = 0; i < NUM_CHOICES; i++) {
        yyjson_mut_arr_add_str(doc, choices, q->choices[i]);
    }
    yyjson_mut_obj_add_val(doc, root, "choices", choices);

    char* question_json = yyjson_mut_write(doc, YYJSON_WRITE_PRETTY, NULL);
    yyjson_mut_doc_free(doc);

    return question_json; // Caller must free this memory
}

