//
// Created by slahmer on 12/31/19.
//

#include <stdio.h>
#include "ftpsock.h"

#include <stdlib.h>
#include <asm/errno.h>

_datasocket datasocket;
_controlsocket controlsocket;

void init_data_socket(){
     datasocket.fd = socket(AF_INET, SOCK_STREAM, 0);
    if (datasocket.fd  == -1) {
        fprintf(stderr,"socket creation failed...\n");
        exit(0);
    }
    else
        printf("Datasocket was successfully created\n");
    bzero(&datasocket._host_addr, sizeof(datasocket._host_addr));
}
void open_data_connection(char* host, uint16_t port){
    init_data_socket();
    datasocket._host = gethostbyname(host);
    if(datasocket._host == NULL){
        close(datasocket.fd);
        fprintf(stderr,"[-] Lookup Failed: %s\n", hstrerror(ECONNREFUSED));
        exit(1);
    }
    memset((char*)&datasocket._host_addr,0,sizeof(datasocket._host_addr));
    datasocket._host_addr.sin_family = AF_INET;
    memcpy((char*) &datasocket._host_addr.sin_addr.s_addr, (char*) datasocket._host->h_addr , datasocket._host->h_length);
    datasocket._host_addr.sin_port = htons(port);
    if (connect(datasocket.fd, (struct sockaddr*) &datasocket._host_addr, sizeof(datasocket._host_addr)) != 0) {
        close(datasocket.fd);
        fprintf(stderr,"[-] Can't connect to host\n");
        exit(1);
    }
    socklen_t len = sizeof(datasocket._myAddr);
    if( getsockname(datasocket.fd, (struct sockaddr*)&datasocket._myAddr, &len) < 0){
        close(datasocket.fd);
        fprintf(stderr,"[-] getsockname Error\n");
        exit(1);
    }
}
void destroy_data_socket(){
    close(datasocket.fd);
}
void send_message(const char* msg){

}
int receive_message(char * msg){
    if(msg == NULL){
        fprintf(stderr,"[-] received msg Error : msg = NULL\n");
        exit(1);
    }
    memset(msg, 0, MAX_BUFF_SIZE);
    ssize_t n;
    n = read(datasocket.fd,msg, MAX_BUFF_SIZE);
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