/* Common file for server & client */

#include "tftp.h"
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

extern int mode;
extern int pack_num;

void send_file(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, char *filename) 
{
    // Implement file sending logic here
    tftp_packet pack;
    pack_num =1;
   
    //OPEN FILE IN READ MODE
    int fd = open(filename,O_RDONLY);
    memset(&pack,0,sizeof(pack));
    //MODE=1 NORMAL MODE

    if(mode == 1)
    {
        int bytes;
        do
        {
            //CLEARING STRUCTURE FOR EACH DATA
            memset(&pack,0,sizeof(pack));
            bytes = read(fd,pack.body.data_packet.data,512);
            //pack.body.data_packet.data_len = bytes;
            printf("bytes read from file:%d\n",bytes);
            int pack_size = bytes+4+2;
            pack.body.data_packet.block_number = pack_num;
            pack.opcode = DATA;
            //copy the data
            tftp_packet temp;
            memcpy(&temp,&pack,sizeof(pack));

            do
            {
                //send bytes
                printf("before sent\n");
                sendto(sockfd,&temp,pack_size,0,(struct sockaddr*)&client_addr,client_len);
                printf("sent from server/n");
                memset(&pack,0,sizeof(pack));
                //rec ack
                recvfrom(sockfd,&pack,sizeof(pack),0,(struct sockaddr*)&client_addr,&client_len);
                printf("recv from client\n");
            }while(pack.opcode !=ACK  || pack.body.ack_packet.block_number != pack_num);//checks pack number recev or not
            //increment pack number
            pack_num++;
            //check eof
            
        }while(bytes == 512);
    }
    else if(mode == 2)
    {
        int bytes;

        do
        {
            memset(&pack,0,sizeof(pack));
            bytes = read(fd,pack.body.data_packet.data,1);
           // pack.body.data_packet.data_len = bytes;
            pack.body.data_packet.block_number = pack_num;
            pack.opcode = DATA;
            //copy the data
            tftp_packet temp;
            memcpy(&temp,&pack,sizeof(pack));

            do
            {
                //send bytes
                sendto(sockfd,&temp,sizeof(temp),0,(struct sockaddr*)&client_addr,client_len);
                memset(&pack,0,sizeof(pack));
                //rec ack
                recvfrom(sockfd,&pack,sizeof(pack),0,(struct sockaddr*)&client_addr,&client_len);
            }while(pack.body.ack_packet.block_number != pack_num);//checks pack number recev or not
            //increment pack number
            pack_num++;
            //check eof
        }while(bytes == 1);
    }
    else if(mode == 3)
    {
         int bytes;

        do
        {
            memset(&pack,0,sizeof(pack));
            //BUFFER TO STORE DATA 512 BYTES
            char buffer[512];
            memset(buffer,0,512);
            pack.body.data_packet.block_number = pack_num;
            pack.opcode = DATA;
            //read data to include \r before \n
            int i;
            char ch;
            for(i=0;i<512 && read(fd,&ch,1);)
            {
                if(ch == '\n')//check character is \n
                {
                    buffer[i++] = '\r';
                }
                buffer[i++] = ch;
            }
            //copying data from buffer to data_packet
            memcpy(pack.body.data_packet.data,buffer,i);
            //copy the data
            tftp_packet temp;
            memcpy(&temp,&pack,sizeof(pack));
            bytes = i;
            //pack.body.data_packet.data_len = bytes;

            do
            {
                //send bytes
                sendto(sockfd,&temp,sizeof(temp),0,(struct sockaddr*)&client_addr,client_len);
                memset(&pack,0,sizeof(pack));
                //rec ack
                recvfrom(sockfd,&pack,sizeof(pack),0,(struct sockaddr*)&client_addr,&client_len);
            }while(pack.body.ack_packet.block_number != pack_num);//checks pack number recev or not
            //increment pack number
            pack_num++;
            //check eof
        }while(bytes == 512);
    }
    close(fd);
    printf("FILE SENT SUCCESSFULLY...\n");
}

void receive_file(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, char *filename) 
{
    // Implement file receiving logic here
    //opening the file and clearing the data
    int fd = open(filename,O_WRONLY | O_TRUNC);
    tftp_packet pack;

    memset(&pack,0,sizeof(pack));
    pack.opcode = ACK;
    //WRITE MODE
    pack.body.ack_packet.block_number = WRITE;
    // send ACK
    sendto(sockfd,&pack,sizeof(pack),0,(struct sockaddr*)&client_addr,client_len);
    pack_num = 1;
    //if mode is normal or netascii
    if(mode == 1 || mode == 3)
    {
        int recv_bytes;
        //receive the data untill it fails condition
        do
        {
            memset(&pack,0,sizeof(pack));
            //receiveing the data
            recv_bytes = recvfrom(sockfd,&pack,sizeof(pack),0,(struct sockaddr*)&client_addr,&client_len);
            //write data to file
            if(pack.opcode == DATA && pack.body.data_packet.block_number == pack_num)
            {
            int data_size = recv_bytes - 6;
            write(fd,pack.body.data_packet.data,data_size);
            memset(&pack,0,sizeof(pack));
            pack.body.ack_packet.block_number = pack_num;
            pack.opcode = ACK;
            //send ACK
            sendto(sockfd,&pack,sizeof(pack),0,(struct sockaddr*)&client_addr,client_len);
            printf("Server sending ACK block = %d\n",pack.body.ack_packet.block_number);
            
            pack_num++;
            }

        }while(recv_bytes == 512);
    }
    else if(mode == 2)
    {
         int recv_bytes;
        //receive the data untill it fails condition
        do
        {
            memset(&pack,0,sizeof(pack));
            //receiveing the data
            recv_bytes = recvfrom(sockfd,&pack,sizeof(pack),0,(struct sockaddr*)&client_addr,&client_len);
            //write data to file
           
            if(pack.opcode == DATA && pack.body.data_packet.block_number == pack_num)
            {
            int data_size = recv_bytes - 6;
            write(fd,pack.body.data_packet.data,data_size);
            memset(&pack,0,sizeof(pack));
            pack.body.ack_packet.block_number = pack_num;
            pack.opcode = ACK;
            printf("Server sending ACK block = %d\n",pack.body.ack_packet.block_number);
            //send ACK
            sendto(sockfd,&pack,sizeof(pack),0,(struct sockaddr*)&client_addr,client_len);
            printf("Server sending ACK block = %d\n",pack.body.ack_packet.block_number);
            
            pack_num++;
            }

        }while(recv_bytes == 1);
    }
    close(fd);
    printf("FILE RECEVIED SUCCESSFULLY...\n");
}