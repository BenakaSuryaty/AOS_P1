#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define ERROR (-1)
#define SERVER_PORT1 4000
#define SERVER_PORT2 4001
#define SERVER_PORT3 4002
#define SERVER_IP "127.0.0.1"
#define CHUNK_SIZE 100

int check(int exp, const char *msg);
void request_file(int server_port, const char* filename);

int main() {
    char filename[256];
    printf("Enter the file name: ");
    scanf("%s", filename);

    // Try connecting to the first server
    request_file(SERVER_PORT1, filename);
    // Try connecting to the second server
    request_file(SERVER_PORT2, filename);
    // Try connecting to the third server
    request_file(SERVER_PORT3, filename);

    printf("File not found on any server.\n");
    return 0;
}

void request_file(int server_port, const char* filename) {
    int client_sock;
    struct sockaddr_in server_addr;
    char buffer[256];

    check((client_sock = socket(AF_INET, SOCK_STREAM, 0)), "Failed to create socket");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr.s_addr);

    check(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)), "Connection failed");

    send(client_sock, filename, strlen(filename), 0);
    recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    buffer[strlen(buffer)] = '\0';

    if (strcmp(buffer, "YES") == 0) {
        printf("File found on server %d. Fetching...\n", server_port);
        
        // Request the file contents
        send(client_sock, "SEND", 4, 0);

        FILE* file = fopen(filename, "wb");
        if (!file) {
            perror("Error creating file");
            close(client_sock);
            exit(1);
        }

        int bytes_received;
        while ((bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
            buffer[bytes_received] = '\0';
            fwrite(buffer, 1, bytes_received, file);
        }

        fclose(file);
        printf("File successfully fetched from server %d\n", server_port);
        close(client_sock);
        exit(0);
    }

    close(client_sock);
}

int check(int exp, const char *msg) {
    if (exp == ERROR) {
        perror(msg);
        exit(1);
    } 
    return exp;
}
