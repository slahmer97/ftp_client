//
// Created by slahmer on 12/31/19.
//

#ifndef FTP_CLIENT_FTPSOCK_H
#define FTP_CLIENT_FTPSOCK_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <memory.h>
#include <fcntl.h>
#define MAX_BUFF_SIZE 512

typedef struct  __socket__ _con_socket;
typedef struct __controlsocket__ _controlsocket;
struct __socket__ {
    int fd;
    struct sockaddr_in _host_addr;
    struct hostent* _host;
    struct sockaddr_in _myAddr;

};

/*
struct __controlsocket__{
    int fd;
    struct sockaddr_in _host_addr;
    unsigned short _port;
    uint32_t _addr;
};
 */

void init_data_socket();
int receive_message(char *);

void open_data_connection(char* host,uint16_t port);
void destroy_data_socket();
void send_message(const char*);
void receive_data(char*);
int send_file(const char*);
int receive_file(char*);




#endif //FTP_CLIENT_FTPSOCK_H
