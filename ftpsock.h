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
#define MAX_BUFF_SIZE 512*4+1

typedef struct __socket__ _con_socket;
typedef struct __dsock__ _d_sock;

struct __socket__ {
	int fd;
	struct sockaddr_in _host_addr;
	struct hostent *_host;
	struct sockaddr_in _myAddr;
};

struct __dsock__ {
	int fd;
	struct sockaddr_in _host_addr;
	uint16_t _port;
	uint32_t _addr;
};

_con_socket *getsockaddr();
uint16_t get_aport();
uint32_t get_aaddr();
int get_afd();

int receive_message(char *);

void open_data_connection(_con_socket * zsock, char *host, uint16_t port);
void destroy_data_socket();
void send_message(const char *);

int open_act_connection();
int open_pasv_connection();

void close_data_connection();
void init_das();


#endif				//FTP_CLIENT_FTPSOCK_H
