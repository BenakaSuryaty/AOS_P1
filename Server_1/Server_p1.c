// standard imports
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

// for socket api and functionality
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

// header to store address info
#include <netinet/in.h>

// NOTE: globals

#define ERROR (-1)
#define SERVER_PORT 4000
#define SERVER_IP "127.0.0.1"
#define SERVER_BACKLOG 6
#define CHUNK_SIZE 100

// struct def to store address info
typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

// Struct to store file read contents.
typedef struct {
    char** chunks;
    size_t chunk_count;
} FileContents;

// Function prototypes
FileContents* get_file_contents(const char* filename);
void free_file_contents(FileContents* file_contents);
int check(int exp, const char *msg);
void handle_client(int client_sock);


int main(){

    // socket creation
    int server_sock, net_sock, addr_size;
    SA_IN server_addr, net_addr;

    check((server_sock = socket(AF_INET, SOCK_STREAM, 0)), "ERROR: Failed to create socket.\n");

    // initializing addr struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr.s_addr);

    check((bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr))), "ERROR: Failed to bind.\n");

    check((listen(server_sock, SERVER_BACKLOG)), "Listen failed.");

    char n_addr[INET_ADDRSTRLEN]; // to store connection's address

    printf("LOG: Waiting for connections...\n");

    // waiting and accepting connections
    addr_size = sizeof(SA_IN);
    check(net_sock = accept(server_sock, (SA *)&net_addr, (socklen_t *)&addr_size),"ERROR: Accept failed!\n");


    inet_ntop(AF_INET, &net_addr.sin_addr, n_addr, INET_ADDRSTRLEN);
    printf("\nLOG: Client with IP:%s connected!\n", n_addr);

    handle_client(net_sock);
    
    close(server_sock);
    return 0;

}


// function to check the errors
int check(int exp, const char *msg)
{
    if (exp == ERROR)
    {
        perror(msg);
        exit(1);
    } 
    return exp;
}


// funciton to handle client
void handle_client(int client_sock) {
    char buffer[256], filename[256];
    memset(buffer, 0, sizeof(buffer));
    memset(filename, 0, sizeof(filename));

    // Receive the filename request from the client
    int bytes_received;

    check((bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0)),"ERROR: Failed to receive filename\n");
    
    if(strncmp(buffer,"FOUND", bytes_received) == 0){
        close(client_sock);
        exit(0);
    }

    buffer[bytes_received] = '\0';
    printf("LOG: Requested file: %s\n", buffer);
    memcpy(filename, buffer, bytes_received + 1);

    // Check if the file exists and read its contents
    FileContents* file_contents = get_file_contents(buffer);

    if (file_contents == NULL) {
        // Send "NO" message if the file doesn't exist
        printf("LOG: File %s not found; Sending 'NO'\n",buffer);
        send(client_sock, "NO", 2, 0);
        close(client_sock);

    } else {
        // Send "YES" message if the file exists
        printf("LOG: File %s found; Sending 'YES'\n",filename);
        send(client_sock, "YES", 3, 0);

        // Wait for the client's request to send the file contents
        memset(buffer, 0, sizeof(buffer));
        bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received < 0) {
            perror("ERROR: Failed to receive request for file contents\n");
            free_file_contents(file_contents);
            return;
        }

        // Send the file contents in chunks
        for (size_t i = 0; i < file_contents->chunk_count; i++) {
            send(client_sock, file_contents->chunks[i], strlen(file_contents->chunks[i]), 0);
        }

        printf("LOG: File %s's contents sent successfully.\n",filename);
        free_file_contents(file_contents);
    }
}


FileContents* get_file_contents(const char* filename) {
    // Construct the full path
    char full_path[1024];
    char dir_path[100]="/home/ben/aos_projects/su_p1/Server_1";
    snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, filename);

    // Check if the file exists
    FILE* file = fopen(full_path, "r");
    if (!file) {
        return NULL;
    }

    // Determine the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Calculate the number of chunks needed
    size_t chunk_count = (file_size + CHUNK_SIZE - 1) / CHUNK_SIZE;

    // Allocate memory for the chunks
    FileContents* file_contents = (FileContents*)malloc(sizeof(FileContents));
    if (!file_contents) {
        perror("ERORR: Error allocating memory for <file_contents>\n");
        fclose(file);
        return NULL;
    }
    file_contents->chunks = (char**)malloc(chunk_count * sizeof(char*));
    if (!file_contents->chunks) {
        perror("ERROR: Error allocating memory for <chunks>\n");
        free(file_contents);
        fclose(file);
        return NULL;
    }
    file_contents->chunk_count = chunk_count;

    // Read the file in chunks
    for (size_t i = 0; i < chunk_count; i++) {
        file_contents->chunks[i] = (char*)malloc(CHUNK_SIZE + 1);
        if (!file_contents->chunks[i]) {
            perror("ERROR: Error allocating memory for <chunks>\n");
            for (size_t j = 0; j < i; j++) {
                free(file_contents->chunks[j]);
            }
            free(file_contents->chunks);
            free(file_contents);
            fclose(file);
            return NULL;
        }
        size_t bytes_read = fread(file_contents->chunks[i], 1, CHUNK_SIZE, file);
        file_contents->chunks[i][bytes_read] = '\0'; // Null-terminate the string
    }

    fclose(file);
    return file_contents;
}


void free_file_contents(FileContents* file_contents) {
    for (size_t i = 0; i < file_contents->chunk_count; i++) {
        free(file_contents->chunks[i]);
    }
    free(file_contents->chunks);
    free(file_contents);
}