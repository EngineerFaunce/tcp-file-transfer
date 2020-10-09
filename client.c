/* Client code */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT_NUMBER     5000
#define SERVER_ADDRESS  "1.22.333.4444"    // replace with IP address of server machine
#define NEW_FILENAME    "new-file.txt"
#define CHUNK_SIZE      1024

int main(int argc, char **argv)
{
    int client_socket;
    struct sockaddr_in remote_addr;
    char buffer[CHUNK_SIZE] = {0};
    long int file_size;
    int remain_data = 0;
    int total_recv = 0;

    /* Zeroing remote_addr struct */
    memset(&remote_addr, 0, sizeof(remote_addr));

    /* Construct remote_addr struct */
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(PORT_NUMBER);
    inet_pton(AF_INET, SERVER_ADDRESS, &(remote_addr.sin_addr));

    /* Create client socket */
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    /* Connect to the server */
    if (connect(client_socket, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) < 0)
    {
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }
    printf("Client connected to server at port %d\n", PORT_NUMBER);

    /* Receiving file size */
    printf("Receiving message from server.\n");
    int recv_size = recv(client_socket, buffer, sizeof(long) + 1, 0);
    file_size = atol(buffer);
    printf("\nFile size : %ld\n", file_size);

    /* Open a new file */
    FILE *fp = fopen(NEW_FILENAME, "w+");
    if (fp == NULL)
    {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    while (total_recv < file_size)
    {
        recv_size = recv(client_socket, buffer, CHUNK_SIZE, 0);
        total_recv += recv_size;
        printf("Received %d Bytes. Total is %d\n", recv_size, total_recv);
		fwrite(buffer, recv_size, 1, fp);
    }
    fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	printf("New file size: %ld\n", file_size);

    fclose(fp);
    close(client_socket);

    return 0;
}