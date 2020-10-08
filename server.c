/* Server code */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>

#define PORT_NUMBER     5000
#define SERVER_ADDRESS  "192.168.1.7"
#define FILE_TO_SEND    "File.HW2.txt"

int main(int argc, char **argv)
{
        int server_socket;
        int peer_socket;
        socklen_t       sock_len;
        ssize_t len;
        struct sockaddr_in      server_addr;
        struct sockaddr_in      peer_addr;
        int fd;
        int sent_bytes = 0;
        char file_size[1024];
        struct stat file_stat;
        int offset;
        int remain_data;

        /* Create server socket */
        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket == -1)
        {
                perror("Error creating socket");
                exit(EXIT_FAILURE);
        }

        /* Zeroing server_addr struct */
        memset(&server_addr, 0, sizeof(server_addr));

        /* Construct server_addr struct */
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT_NUMBER);
        inet_pton(AF_INET, SERVER_ADDRESS, &(server_addr.sin_addr));

        if ((bind(server_socket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))) < 0)
        {
                perror("Bind failed. Error");
                exit(EXIT_FAILURE);
        }
        printf("Bind completed.\n");

        /* Listening to incoming connections */
        listen(server_socket, 1);

        fd = open(FILE_TO_SEND, O_RDONLY);
        if (fd < 0)
        {
                perror("File error");
                exit(EXIT_FAILURE);
        }

        /* Get file stats */
        if (fstat(fd, &file_stat) < 0)
        {
                perror("Filestat error");
                exit(EXIT_FAILURE);
        }

        fprintf(stdout, "File Size: \n%d bytes\n", file_stat.st_size);

        sock_len = sizeof(struct sockaddr_in);
        /* Accepting incoming peers */
        peer_socket = accept(server_socket, (struct sockaddr *)&peer_addr, &sock_len);
        if (peer_socket < 0)
        {
                perror("Accept failed.");
                exit(EXIT_FAILURE);
        }

        sprintf(file_size, "%d", file_stat.st_size);

        /* Sending file size */
        len = send(peer_socket, file_size, sizeof(file_size), 0);
        printf("Server sent %d bytes for the size\n", len);

        offset = 0;
        remain_data = file_stat.st_size;
        /* Sending file data */
        while (((sent_bytes = sendfile(peer_socket, fd, &offset, 1024)) > 0) && (remain_data > 0))
        {
                fprintf(stdout, "1. Server sent %d bytes from file's data, offset is now : %d and remaining data = %d\n", sent_bytes, offset, remain_data);
                remain_data -= sent_bytes;
                fprintf(stdout, "2. Server sent %d bytes from file's data, offset is now : %d and remaining data = %d\n", sent_bytes, offset, remain_data);
        }

        close(peer_socket);
        close(server_socket);

        return 0;
}