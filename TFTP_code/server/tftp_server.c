#include "tftp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>

int pack_num = 1;
int mode = 1;
void handle_client(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, tftp_packet *packet);

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    tftp_packet packet;


    // Create UDP socket
    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // Bind the socket
    bind(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr));
    // Set socket timeout option
    //TODO Use setsockopt() to set timeout option
  

    printf("TFTP Server listening on port %d...\n", PORT);

    // Main loop to handle incoming requests
    while (1) {
        printf("Waiting for packets\n");
        int n = recvfrom(sockfd, &packet, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);
        printf("Opcode = %d\n", packet.opcode);
        if (n < 0) {
            perror("Receive failed or timeout occurred");
            continue;
        }
        printf("received %d bytes\n",n);
         handle_client(sockfd, client_addr, client_len, &packet);
    }

    close(sockfd);
    return 0;
}

void handle_client(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, tftp_packet *packet) 
{
    // Extract the TFTP operation (read or write) from the received packet
    // and call send_file or receive_file accordingly
    mode = packet->body.request.mode;

    pack_num = 1;

    if(packet->opcode == WRQ)
    {
        int fd = open(packet->body.request.filename,O_EXCL | O_CREAT | O_WRONLY,0644);
        if(fd == -1)
        {
            fd = open(packet->body.request.filename,O_WRONLY|O_TRUNC);
        }
        close(fd);

        receive_file(sockfd,client_addr,client_len,packet->body.request.filename);
        return;
    }
    else if(packet->opcode == RRQ)
    {
        int fd = open(packet->body.request.filename,O_EXCL | O_CREAT | O_RDONLY,0644);
        if(fd == -1)
        {
            close(fd);

            send_file(sockfd,client_addr,client_len,packet->body.request.filename);
            return;
        }
        else
        {
            memset(packet,0,sizeof(*packet));
            packet->opcode = ERROR;
            strcpy(packet->body.error_packet.error_msg,"FILE IS NOT CREATED\n");

            sendto(sockfd,packet,sizeof(*packet),0,(struct sockaddr*)&client_addr,client_len);
        }
    }

}




