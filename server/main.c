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

#include "questions.h"
#include "types.h"
#include "yyjson.h"

#define BUFFER_SIZE 1024
#define NUM_CHOICES 4 
#define DB_PATH "data/trains.db"
#define PORT 8080
#define MAX_CONNECTIONS 5

int handle_client(int client_sock, sqlite3* db);
char *question_to_json(Question *q);

int main(void) {
    // We need to seed our rng
    srand(time(NULL));

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
            continue; // Go back to accepting new clients
        } 

        // Child process
        // What do you mean this is cursed indent ?
        if (pid == 0) {
            close(server_sock); // Child doesn't need the listening socket
            return handle_client(client_sock, db);
        }

        // Parent process
        close(client_sock); // Parent doesn't need the client socket
                            // No waitpid here to avoid blocking the parent
    }

    close(server_sock);
    sqlite3_close(db);
    return EXIT_SUCCESS;
}

int handle_client(int client_sock, sqlite3* db) {
    // Reseed the RNG in the child process
    // I had to do this to get random questions
    srand(time(NULL) ^ getpid());

    for(int i = 0; i < 4; i++){

        // Receive message from client
        // Not needed to read the value
        // It's only to know when the client answered
        char buffer_useless[BUFFER_SIZE];
        memset(buffer_useless, 0, BUFFER_SIZE);
        if (recv(client_sock, buffer_useless, BUFFER_SIZE, 0) <= 0) {
            perror("recv");
            close(client_sock);
            exit(EXIT_FAILURE);
        }
        // printf("Client: %s\n", buffer_useless);

        printf("Sending Question %d\n", i+1);
        Question question = generate_question(db);
        // Receive message from client
        char buffer[BUFFER_SIZE];           

        const char *question_json = question_to_json(&question);

        // Send response to client
        snprintf(buffer, BUFFER_SIZE, "%s", question_json);
        if (send(client_sock, buffer, strlen(buffer), 0) == -1) {
            perror("send");
            exit(EXIT_FAILURE);
        }
    }
    close(client_sock);
    exit(EXIT_SUCCESS);
}

char *question_to_json(Question *q) {
    // Create a mutable JSON document
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_val *root = yyjson_mut_obj(doc);
    yyjson_mut_doc_set_root(doc, root);

    // Add fields to the JSON object
    yyjson_mut_obj_add_str(doc, root, "question", q->prompt);
    yyjson_mut_obj_add_int(doc, root, "correct_answer", q->correct_answer);

    // Add the "solutions" array
    yyjson_mut_val *choices = yyjson_mut_arr(doc);
    for (size_t i = 0; i < NUM_CHOICES; i++) {
        yyjson_mut_arr_add_str(doc, choices, q->choices[i]);
    }
    yyjson_mut_obj_add_val(doc, root, "choices", choices);

    // Write JSON to string
    char *question_json = yyjson_mut_write(doc, YYJSON_WRITE_PRETTY, NULL);

    yyjson_mut_doc_free(doc);

    return question_json; 
}



