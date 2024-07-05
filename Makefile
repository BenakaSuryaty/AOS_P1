# Makefile for Project_1

CC = gcc
CFLAGS = -Wall -g  # Adjust compiler flags as needed

# Directories
CLIENT_DIR = Client
SERVER_1_DIR = Server_1
SERVER_2_DIR = Server_2
SERVER_3_DIR = Server_3

# Executables
CLIENT_EXEC = Client_p4
SERVER_1_EXEC = Server_p1
SERVER_2_EXEC = Server_p2
SERVER_3_EXEC = Server_p3

# Source files
CLIENT_SRC = $(CLIENT_DIR)/Client_p4.c
SERVER_1_SRC = $(SERVER_1_DIR)/Server_p1.c
SERVER_2_SRC = $(SERVER_2_DIR)/Server_p2.c
SERVER_3_SRC = $(SERVER_3_DIR)/Server_p3.c
FILE_CREATE_SRC = file_create.c

# Targets
all: $(CLIENT_EXEC) $(SERVER_1_EXEC) $(SERVER_2_EXEC) $(SERVER_3_EXEC) file_create

$(CLIENT_EXEC): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT_DIR)/$(CLIENT_EXEC) $(CLIENT_SRC)

$(SERVER_1_EXEC): $(SERVER_1_SRC)
	$(CC) $(CFLAGS) -o $(SERVER_1_DIR)/$(SERVER_1_EXEC) $(SERVER_1_SRC)

$(SERVER_2_EXEC): $(SERVER_2_SRC)
	$(CC) $(CFLAGS) -o $(SERVER_2_DIR)/$(SERVER_2_EXEC) $(SERVER_2_SRC)

$(SERVER_3_EXEC): $(SERVER_3_SRC)
	$(CC) $(CFLAGS) -o $(SERVER_3_DIR)/$(SERVER_3_EXEC) $(SERVER_3_SRC)

file_create: $(FILE_CREATE_SRC)
	$(CC) $(CFLAGS) -o file_create $(FILE_CREATE_SRC)
	./file_create

clean:
	rm -f $(CLIENT_DIR)/$(CLIENT_EXEC) \
		  $(SERVER_1_DIR)/$(SERVER_1_EXEC) \
		  $(SERVER_2_DIR)/$(SERVER_2_EXEC) \
		  $(SERVER_3_DIR)/$(SERVER_3_EXEC) \
		  file_create
	
	rm -f $(CLIENT_DIR)/*.txt \
	      $(SERVER_1_DIR)/*.txt \
	      $(SERVER_2_DIR)/*.txt \
	      $(SERVER_3_DIR)/*.txt

.PHONY: all clean