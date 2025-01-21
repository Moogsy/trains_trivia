#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "utils.h"
#include "types.h"

#define PORT 8080
#define BUFFER_SIZE 1024

void print_question(Question *q, int index, char *buffer);

int main() {
    // Create socket
    int client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(client_sock);
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(client_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(client_sock);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server\n");

    for(int i = 0; i < 4; i++){

         // Send message to server
        char buffer[BUFFER_SIZE];
        snprintf(buffer, BUFFER_SIZE, "Send me a question %d", i + 1);
        if (send(client_sock, buffer, strlen(buffer), 0) == -1) {
            perror("send");
            close(client_sock);
            exit(EXIT_FAILURE);
        }

        // Receive response from server
        memset(buffer, 0, BUFFER_SIZE);
        if (recv(client_sock, buffer, BUFFER_SIZE, 0) <= 0) {
            perror("recv");
            close(client_sock);
            exit(EXIT_FAILURE);
            }
        Question q;
        print_question(&q, i, buffer);      

    }

    close(client_sock);
    return 0;
}

void print_question(Question *q, int index, char *buffer){
    if (json_to_question(buffer, q) == 0) {
        printf("Question Number %d\n ", index+1);
        printf("Question: %s\n ", q->prompt);
        printf("Choices:\n");
        for (int i = 0; i < NUM_CHOICES; i++) {
            printf("  %d: %s\n", i+1, q->choices[i]);
        }

        int answer;
        printf("Choose an answer: \n");
        scanf("%d", &answer);

        char* message = answer-1 == q->correct_answer? "Correct! " : "Wrong! ";
        printf("%s The answer is: %s\n", message, q->choices[q->correct_answer]);

    }
}

