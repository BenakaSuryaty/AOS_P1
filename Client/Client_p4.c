#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>

#define ERROR (-1)
#define SERVER_1_PORT 4000
#define SERVER_1_IP "127.0.0.1"

#define SERVER_2_PORT 4001
#define SERVER_2_IP "127.0.0.1"

#define SERVER_3_PORT 4002
#define SERVER_3_IP "127.0.0.1"

#define CHUNK_SIZE 100

#define MAX_FILENAME_LENGTH 250
#define MAX_EXTENSION_LENGTH 10

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

int check(int exp, const char *msg);
void trim_whitespace(char *str);
int is_valid_filename(const char *filename);

int request_file(int server_sock, const char* filename, const char* server_name);


// Function to check if the filename is valid
int is_valid_filename(const char *filename) {
    const char *forbidden_chars = "\\/:*?\"<>|$";

    // Check for empty filename
    if (strlen(filename) == 0) {
        printf("ERROR: Filename cannot be empty.\n");
        return 0;
    }

    // Check for filename length
    if (strlen(filename) >= MAX_FILENAME_LENGTH) {
        printf("ERROR: Filename is too long.\n");
        return 0;
    }

    // Check for forbidden characters
    for (const char *p = filename; *p != '\0'; p++) {
        if (strchr(forbidden_chars, *p) != NULL) {
            printf("ERROR: Filename contains forbidden character: %c\n", *p);
            return 0;
        }
    }

    // Check for valid extension (e.g., .txt, .pdf)
    const char *valid_extensions[] = {".txt", ".pdf", ".docx"};
    const char *ext = strrchr(filename, '.');
    if (ext != NULL) {
        int valid_extension = 0;
        for (int i = 0; i < sizeof(valid_extensions) / sizeof(valid_extensions[0]); i++) {
            if (strcmp(ext, valid_extensions[i]) == 0) {
                valid_extension = 1;
                break;
            }
        }
        if (!valid_extension) {
            printf("ERROR: Invalid file extension: %s\n", ext);
            return 0;
        }
    } else {
        printf("ERROR: Filename does not have an extension.\n");
        return 0;
    }

    return 1;
}


// Function to trim leading and trailing whitespace
void trim_whitespace(char *str) {
    char *end;

    // Trim leading space
    while (isspace((unsigned char) *str)) str++;

    if (*str == 0) {  // All spaces
        str[0] = '\0';
        return;
    }

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char) *end)) end--;

    // Write new null terminator
    end[1] = '\0';
}


int request_file(int server_sock, const char* filename,const char* server_name) {
    
    char buffer[256];

    send(server_sock, filename, strlen(filename), 0);
    recv(server_sock, buffer, sizeof(buffer) - 1, 0);
    buffer[strlen(buffer)] = '\0';

    if (strcmp(buffer, "YES") == 0) {
        
        // Request the file contents
        send(server_sock, "SEND", 4, 0);

        FILE* file = fopen(filename, "wb");
        if (!file) {
            perror("Error creating file");
            close(server_sock);
            exit(1);
        }

        int bytes_received;
        while ((bytes_received = recv(server_sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
            buffer[bytes_received] = '\0';
            fwrite(buffer, 1, bytes_received, file);
        }

        fclose(file);
        printf("File successfully fetched from %s\n", server_name);
        close(server_sock);

        return 1;
    }
    else
    {
        return 0;
    }
    
}

int check(int exp, const char *msg) {
    if (exp == ERROR) {
        perror(msg);
        exit(1);
    } 
    return exp;
}


int main() {
    char filename[256];
    int ServerSock1, ServerSock2, ServerSock3;
    SA_IN server_addr;
    
    printf("LOG: Starting Client.\n");
    
    check((ServerSock1 = socket(AF_INET, SOCK_STREAM, 0)), "ERROR: Failed to create Server 1 socket \n");
    check((ServerSock2 = socket(AF_INET, SOCK_STREAM, 0)), "ERROR: Failed to create Server 2 socket \n");
    check((ServerSock3 = socket(AF_INET, SOCK_STREAM, 0)), "ERROR: Failed to create Server 3 socket \n");

    printf("LOG: Establishing connection with the servers\n");

    //NOTE: SERVER 1
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_1_PORT); 
    inet_pton(AF_INET, SERVER_1_IP, &server_addr.sin_addr.s_addr);

    check(connect(ServerSock1, (SA*)&server_addr, sizeof(server_addr)),"ERROR: Connection with Server 1 failed! \n");

    printf("LOG: Server 1 Connected!\n");

    //NOTE: SERVER 2
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_2_PORT); 
    inet_pton(AF_INET, SERVER_2_IP, &server_addr.sin_addr.s_addr);

    check(connect(ServerSock2, (SA*)&server_addr, sizeof(server_addr)),"ERROR: Connection with Server 2 failed! \n");

    printf("LOG: Server 2 Connected!\n");

    //NOTE: SERVER 3
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_3_PORT); 
    inet_pton(AF_INET, SERVER_2_IP, &server_addr.sin_addr.s_addr);

    check(connect(ServerSock3, (SA*)&server_addr, sizeof(server_addr)),"ERROR: Connection with Server 3 failed! \n");

    printf("LOG: Server 3 Connected!\n");

retry_fn:
    printf("Enter a filename: ");
    if (fgets(filename, sizeof(filename), stdin) != NULL) {
        // Remove the newline character if it exists
        size_t len = strlen(filename);
        if (len > 0 && filename[len - 1] == '\n') {
            filename[len - 1] = '\0';
        }

        // Trim leading and trailing whitespace
        trim_whitespace(filename);

        if (is_valid_filename(filename)) {
                printf("Searching for file '%s'...\n", filename);

                if (request_file(ServerSock1, filename,"Server 1") == 1) {
                    exit(0);
                } else if (request_file(ServerSock2, filename, "Server 2") == 1) {
                    exit(0);
                } else if (request_file(ServerSock3, filename, "Server 3") == 1) {
                    exit(0);
                } else {
                    printf("File not found.\n");
                    close(ServerSock1);
                    close(ServerSock2);
                    close(ServerSock3);
                    exit(0);
                }
        } else {
            printf("LOG: Filename '%s' is invalid; Retry \n", filename);
            goto retry_fn;
        }
    } else {
        printf("Error reading input.\n");
    }

    return 0;
}






