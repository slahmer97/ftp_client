//
// Created by slahmer on 12/31/19.
//

#include <stdio.h>
#include "ftpsock.h"
#include "ftpc.h"

#include <stdlib.h>
#include <asm/errno.h>
#include <errno.h>

_con_socket sock;

_con_socket p_socket;
_d_sock a_socket;


/*
* Cette fonction initialise une connexion TCP et l'enregistre dans "fd".
*/
static void init_tcp_socket(int *fd, struct sockaddr_in *_host_addr)
{
	*fd = socket(AF_INET, SOCK_STREAM, 0);
	if (*fd == -1) {
		fprintf(stderr, "socket creation failed...\n");
		exit(0);
	} else
		printf("Datasocket was successfully created\n");
	bzero(_host_addr, sizeof(*_host_addr));
}

/*
* Cette fonction initialise une connexion avec
* un serveur FTP "host" sur "port".
*/
void open_data_connection(char *host, uint16_t port)
{
	init_tcp_socket(&sock.fd, &sock._host_addr);
	sock._host = gethostbyname(host);
	if (sock._host == NULL) {
		close(sock.fd);
		fprintf(stderr, "[-] Lookup Failed: %s\n",
			hstrerror(ECONNREFUSED));
		return;
	}
	memset((char *)&sock._host_addr, 0, sizeof(sock._host_addr));
	sock._host_addr.sin_family = AF_INET;
	memcpy((char *)&sock._host_addr.sin_addr.s_addr,
	       (char *)sock._host->h_addr, sock._host->h_length);
	sock._host_addr.sin_port = htons(port);
	if (connect
	    (sock.fd, (struct sockaddr *)&sock._host_addr,
	     sizeof(sock._host_addr)) != 0) {
		close(sock.fd);
		perror("[-] Can't connect to host\n");
		return;
	}
	socklen_t len = sizeof(sock._myAddr);
	if (getsockname(sock.fd, (struct sockaddr *)&sock._myAddr, &len) < 0) {
		close(sock.fd);
		perror("[-] getsockname Error\n");
		return;
	}
	fprintf(stdout, "ftp client RFC-959\n");
	fprintf(stdout,
		"Connection established, waiting for welcome message...\n");
}


/*
* Cette fonction ouvre un socket et attend que le serveur FTP se connecte.
*/
int open_act_connection()
{
	init_tcp_socket(&a_socket.fd, &a_socket._host_addr);
	a_socket._host_addr = sock._host_addr;
	a_socket._host_addr.sin_port = htons(0);	//kernel will randomely chooose a free port for us
	if (bind
	    (a_socket.fd, (struct sockaddr *)&a_socket._host_addr,
	     sizeof(a_socket._host_addr)) != 0) {
		//fprintf(stderr, "[-] error when binding socket\n");
		perror("[-] biding failed\n");
		close(a_socket.fd);
		return -1;
	}

	if (listen(a_socket.fd, 1) != 0) {	// allow just server connection {one connection}
		close(a_socket.fd);
		perror("[-] error when listening socket\n");
		return -1;
	}

	socklen_t len = sizeof(a_socket._addr);
	if (getsockname(a_socket.fd, (struct sockaddr *)&sock._myAddr, &len) <
	    0) {
		close(a_socket.fd);
		perror("[-] error when getsockname\n");
		return -1;
	}
	a_socket._port = sock._myAddr.sin_port;
	a_socket._addr = sock._myAddr.sin_addr.s_addr;

	return 1;
}

/*
* Renvoie le descripteur de fichier de donnees "fd".
*/
int get_afd()
{
	return a_socket.fd;
}

/*
* Cette fonction verife avec le serveur FTP si le mode passif est possible.
*/
int open_pasv_connection()
{
	send_message("PASV\r\n");
	char recBUF[MAX_BUFF_SIZE];
	char *input;
	receive_message(recBUF);
	if (_debug_)
		fprintf(stdout, "[<--] %s", recBUF);

	char resp[4];
	resp[3] = 0;
	resp[0] = recBUF[0];
	resp[1] = recBUF[1];
	resp[2] = recBUF[2];
	uint16_t replay_code = (uint16_t) atoi(resp);
	if (replay_code != 227) {
		fprintf(stdout, "[-] Passive mode rejected by server\n");
		return -replay_code;
	}
	input = recBUF + 4;
	while (*(input++) != '(') ;
	char addr[17];
	addr[16] = 0;
	for (int i = 0, index = 0; i != 4; index++) {
		if (*input == ',') {
			if (i != 3)
				addr[index] = '.';
			else
				addr[index] = 0;
			i++;
		} else
			addr[index] = *input;
		input++;
	}

	return -22;
}

/*
* Renvoie le numero de port de la connexion de commande.
*/
uint16_t get_aport()
{
	return a_socket._port;
}

/*
* Renvoie l'adresse de la connexion de commande.
*/
uint32_t get_aaddr()
{
	return a_socket._addr;
}

/*
* Detruit la connexion tcp des donnees.
*/
void destroy_data_socket()
{
	close(sock.fd);
}

/*
* Cette fonction envoie les donnees de "msg" au serveur FTP.
*/
void send_message(const char *msg)
{
	char const *buffer = msg;
	size_t size = strlen(msg);
	size_t num_written_bytes = 0;
	while (num_written_bytes < size) {
		ssize_t success_trans =
		    write(sock.fd, buffer + num_written_bytes,
			  size - num_written_bytes);
		if (success_trans == -1) {
			switch (errno) {
			case EPIPE:
				{
					fprintf(stderr,
						"[-] ERROR occured while transferring data\n");
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

/*
* Cette fonction recoit les donnees du serveur FTP et les enregistrent
* dans "msg".
*/
int receive_message(char *msg)
{
	if (msg == NULL) {
		fprintf(stderr, "[-] received msg Error : msg = NULL\n");
		exit(1);
	}
	memset(msg, 0, MAX_BUFF_SIZE);
	ssize_t n;
	n = read(sock.fd, msg, MAX_BUFF_SIZE);
	if (n < 0) {
		fprintf(stderr, "[-] received msg Error : N < 0 [%zd] \n", n);
		exit(1);
	}
	return n;
}

void receive_data(char *data)
{

}

int send_file(const char *file)
{

	return 0;
}

int receive_file(char *file)
{

	return 0;
}
