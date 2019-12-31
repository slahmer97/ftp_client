//
// Created by slahmer on 12/31/19.
//

#include <stdio.h>
#include "ftpsock.h"

#include <stdlib.h>
#include <asm/errno.h>
#include <errno.h>

_con_socket sock;
//_controlsocket controlsocket;

void init_data_socket(){
    sock.fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock.fd  == -1) {
        fprintf(stderr,"socket creation failed...\n");
        exit(0);
    }
    else
        printf("Datasocket was successfully created\n");
    bzero(&sock._host_addr, sizeof(sock._host_addr));
}
void open_data_connection(char* host, uint16_t port){
    init_data_socket();
    sock._host = gethostbyname(host);
    if(sock._host == NULL){
        close(sock.fd);
        fprintf(stderr,"[-] Lookup Failed: %s\n", hstrerror(ECONNREFUSED));
        exit(1);
    }
    memset((char*)&sock._host_addr,0,sizeof(sock._host_addr));
    sock._host_addr.sin_family = AF_INET;
    memcpy((char*) &sock._host_addr.sin_addr.s_addr, (char*) sock._host->h_addr , sock._host->h_length);
    sock._host_addr.sin_port = htons(port);
    if (connect(sock.fd, (struct sockaddr*) &sock._host_addr, sizeof(sock._host_addr)) != 0) {
        close(sock.fd);
        fprintf(stderr,"[-] Can't connect to host\n");
        exit(1);
    }
    socklen_t len = sizeof(sock._myAddr);
    if( getsockname(sock.fd, (struct sockaddr*)&sock._myAddr, &len) < 0){
        close(sock.fd);
        fprintf(stderr,"[-] getsockname Error\n");
        exit(1);
    }
    fprintf(stdout,"ftp client RFC-959\n");
    fprintf(stdout,"Connection established, waiting for welcome message...\n");
}
void destroy_data_socket(){
    close(sock.fd);
}

void send_message(const char* msg){
    char const* buffer = msg;
    size_t size = strlen(msg);
    size_t num_written_bytes = 0;
    while(num_written_bytes < size) {
        ssize_t success_trans = write(sock.fd, buffer + num_written_bytes, size - num_written_bytes);
        if (success_trans == -1) {
            switch(errno) {
                case EPIPE:
                {
                    fprintf(stderr,"[-] ERROR occured while transferring data\n");
                }
                case EAGAIN:
                {
                    continue;
                }
            }
        }
        num_written_bytes += success_trans;
    }
}
int receive_message(char * msg){
    if(msg == NULL){
        fprintf(stderr,"[-] received msg Error : msg = NULL\n");
        exit(1);
    }
    memset(msg, 0, MAX_BUFF_SIZE);
    ssize_t n;
    n = read(sock.fd,msg, MAX_BUFF_SIZE);
    if (n < 0){
        fprintf(stderr,"[-] received msg Error : N < 0 [%zd] \n",n);
        exit(1);
    }
    return n;
}
void receive_data(char* data){

}
int send_file(const char* file){

    return 0;
}
int receive_file(char* file){


    return 0;
}