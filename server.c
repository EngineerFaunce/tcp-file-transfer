/* Server code */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT_NUMBER     5000
#define SERVER_ADDRESS  "1.22.333.4444" // replace with IP address of server machine
#define FILE_TO_SEND    "file.txt"
#define CHUNK_SIZE      1024

int main(int argc, char **argv)
{
    int server_socket, peer_socket;
    socklen_t sock_len;
    struct sockaddr_in server_addr, peer_addr;
    int send_size, recv_size;
    int actually_read, total_read, num_left;
    int total_sent = 0;
    char buf[4096];

    /* Zeroing server_addr struct */
    memset(&server_addr, 0, sizeof(server_addr));
    
    /* Create sockets */
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    peer_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0 || peer_socket < 0)
    {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
    
    /* Construct server_addr struct */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT_NUMBER);
    inet_pton(AF_INET, SERVER_ADDRESS, &(server_addr.sin_addr));

    /* Binds IP and port number */
    if ((bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr))) < 0)
    {
        perror("Bind failed. Error");
        exit(EXIT_FAILURE);
    }
    printf("Bind completed.\n");

    /* Server socket enters listening mode */
    listen(server_socket, 1);
    printf("Server listening on port %d\n", PORT_NUMBER);


    printf("Waiting for incoming connections...\n");
    sock_len = sizeof(struct sockaddr_in);
    /* Accepting incoming peers */
    peer_socket = accept(server_socket, (struct sockaddr *)&peer_addr, (socklen_t*)&sock_len);
    if (peer_socket < 0)
    {
        perror("Accept failed.");
        exit(EXIT_FAILURE);
    }
    printf("Connection accepted\n");

    /* Open file in read-only */
    FILE *fp = fopen("file.txt", "r");
	if (fp  == NULL)
	{
		perror("File not found");
		exit(EXIT_FAILURE);
	}

    /* Determine file size and send it to client */
    fseek(fp, 0, SEEK_END);
	long int file_size = ftell(fp);
	printf("%ld is the file size.\n", file_size);
	sprintf(buf, "%ld\n", file_size);
	send_size = send(peer_socket, buf, sizeof(long)+1, 0);
	printf("Sent %d bytes\n", send_size);

    /* Send data in chunks to client */
    fseek(fp, 0, SEEK_SET);
    int num_bytes_to_read = CHUNK_SIZE;
    while(!feof(fp))
    {
        // Read chunk from file
        actually_read = fread(buf, 1, CHUNK_SIZE, fp);
        printf("Actually read %d\n", actually_read);
        total_read += actually_read;

        // Send chunk to client
        send_size = send(peer_socket, buf, actually_read, 0);
        total_sent += send_size;
        printf("Read %d, Sent %d bytes, Total sent %d\n", actually_read, send_size, total_sent);
        
        // Check if program needs to update how many bytes to send
        num_left = (int) file_size - total_read;
        if (num_left >= CHUNK_SIZE)
            num_bytes_to_read = CHUNK_SIZE;
        else
            num_bytes_to_read = num_left;
        printf("Total read is %d, Number of bytes left is %d, bytes to read is %d\n", 
            total_read, num_left, num_bytes_to_read);
        
        if (num_left == 0)
            break;
    }
    fclose(fp);
    close(peer_socket);
    close(server_socket);

    return 0;
}