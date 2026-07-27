#include "tftp.h"
#include "tftp_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>

int mode = 1;
int pack_num =1;

int main() {
    char command[256];
    tftp_client_t client;
    memset(&client, 0, sizeof(client));  // Initialize client structure
    
    // Main loop for command-line interface
    while (1) {
        printf("Menu:\n");
        printf("connect <server-ip>     : connect to server\nget <file-name>     : Receive a file from server\nput <file-name>   : Send a file to the server\nmode              : Transfer mode\nExit           : Exit the application\n");

        printf("tftp> ");
       
        char command[10];
        scanf(" %s",command);
        // Remove newline character
        command[strcspn(command, "\n")] = 0;

        // Process the command
        process_command(&client, command);
    }

    return 0;
}

// Function to process commands
void process_command(tftp_client_t *client, char *command)
{
    struct sockaddr_in  sa;
    if(strcmp(command,"connect") == 0)
    {
        char ip[20];
        printf("Enter Server IP Address: \n");
        scanf(" %s",ip);
    
        strcpy(client->server_ip,ip);
        
        connect_to_server(client,client->server_ip,PORT);
    }
    else if(strcmp(command,"put") == 0)
    {
        char filename[30];
        printf("Enter File name: \n");
        scanf(" %s",filename);
        put_file(client,filename);
    }
    else if(strcmp(command,"get") == 0)
    {
        char filename[30];
        printf("Enter File name: \n");
        scanf(" %s",filename);
        get_file(client,filename);
    }
    else if(strcmp(command,"mode") == 0)
    {
        printf("Menu : \n1.Normal\n2.Octet\n3.Netascii\n");
        printf("Enter mode: \n");
        scanf(" %d",&mode);
        
    }
    else if(strcmp(command,"exit") == 0)
    {
        disconnect(client);
        exit(0);
    }
   
}

// This function is to initialize socket with given server IP, no packets sent to server in this function
void connect_to_server(tftp_client_t *client, char *ip, int port) {
    // Create UDP socket
    // Set socket timeout option
    // Set up server address

    if(inet_pton(AF_INET,ip, &client->server_addr.sin_addr) != 1)// it fills the ip address
        {
            printf("Invalid IP address\n");
            
            return;
        }
    client->sockfd = socket(AF_INET,SOCK_DGRAM,0);

    if(client->sockfd == -1)
    {
        printf("Socket not created\n");
        return;
    }
    else
    {
        printf("Socket created\n");
    }
    client->server_addr.sin_family = AF_INET;
    client->server_addr.sin_port = htons(PORT);
    

}

void put_file(tftp_client_t *client, char *filename) {
    // Send WRQ request and send file
    int fd = open(filename,O_RDONLY);
    if(fd == -1)
    {
        printf("File doesnt exist\n");
        return;
    }
    close(fd);

    send_request(client->sockfd,client->server_addr,filename,WRQ);

}

void get_file(tftp_client_t *client, char *filename) {
    // Send RRQ and recive file 
    send_request(client->sockfd,client->server_addr,filename,RRQ);
  receive_file(client->sockfd,
                 client->server_addr,
                 sizeof(client->server_addr),
                 filename);
}

void disconnect(tftp_client_t *client) {
    // close fd
    close(client->sockfd);
   
}
void send_request(int sockfd, struct sockaddr_in server_addr, char *filename, int opcode)
{
    
	tftp_packet pack;
	// clearing the garbage values //
	memset(&pack,0,sizeof(pack));
	// initilizing the variables
	pack.opcode = opcode;
	strcpy(pack.body.request.filename,filename);
	
	pack.body.request.mode = mode;
	// sending request to the server //
    printf("Sending %d\n",opcode);
	int n= sendto(sockfd,&pack,sizeof(pack),0,(struct sockaddr *)&server_addr,sizeof(server_addr));
	printf("bytes sent :%d\n",n);
    // calling the receive_request function //
    if(opcode == WRQ)
	receive_request(sockfd,server_addr,filename,opcode);
}

void receive_request(int sockfd,struct sockaddr_in server_addr, char *filename, int opcode)
{
    tftp_packet pack;
    socklen_t server_len = sizeof(server_addr);
    memset(&pack,0,sizeof(pack));
    recvfrom(sockfd,&pack,sizeof(pack),0,(struct sockaddr*)&server_addr,&server_len);

    if(pack.opcode == ACK)
    {
        if(pack.body.ack_packet.block_number == WRITE)
        {
            send_file(sockfd,server_addr,sizeof(server_addr),filename);
        }
        else if(pack.body.ack_packet.block_number == READ) 
        {
            receive_file(sockfd,server_addr,sizeof(server_addr),filename);
        }
        else
            return;
    }
    else if(pack.opcode == ERROR)
    {
        printf("%s",pack.body.error_packet.error_msg);
    }
}