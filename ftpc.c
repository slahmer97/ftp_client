//
// Created by slahmer on 12/31/19.
//
#include "ftpc.h"
#include <stdlib.h>
#include <stdio.h>
#include <asm/ioctls.h>
#include <sys/ioctl.h>

enum FTP_C_CMD get_command(const char *cmd)
{
	if (strncmp("open", cmd, 4) == 0)
		return OPEN;
	else if (strncmp("dir", cmd, 3) == 0)
		return DIR;
	else if (strncmp("show", cmd, 4) == 0)
		return SHOW;
	else if (strncmp("ciao", cmd, 4) == 0)
		return CIAO;
	else if (strncmp("exit", cmd, 4) == 0)
		return EXIT;
	else if (strncmp("debugon", cmd, 7) == 0)
		return DEBUG_ON;
	else if (strncmp("debugoff", cmd, 8) == 0)
		return DEBUG_OFF;
	else if (strncmp("passiveon", cmd, 9) == 0)
		return PASSIVE_ON;
	else if (strncmp("passiveoff", cmd, 10) == 0)
		return PASSIVE_OFF;
	else if (strncmp("get", cmd, 3) == 0)
		return GET_FILE;
	else if (strncmp("send", cmd, 4) == 0)
		return SEND_FILE;
	else if (strncmp("ren", cmd, 3) == 0)
		return REN_FILE;
	else if (strncmp("del", cmd, 3) == 0)
		return DEL_FILE;
	else if (strncmp("cd", cmd, 2) == 0)
		return CD;
	else if (strncmp("mdir", cmd, 4) == 0)
		return MKDIR;
	else if (strncmp("rdir", cmd, 4) == 0)
		return RMDIR;
	else if (strncmp("hist", cmd, 4) == 0)
		return HISTORY;
	else if (strncmp("help", cmd, 4) == 0)
		return HELP;
	return INVALID_CMD;

}

void passivef(int flag)
{
	_mode_ = flag ? PASSIVE : ACTIVE;
}

void debugf(int flag)
{
	_debug_ = flag;
}

void login(int flag)
{
	_status_ = flag ? CONNECTED : DISCONNECTED;
}

uint16_t send_cmd(const char *cmd, const char *args, int print_cmd)
{
	size_t cmdlen = strlen(cmd);
	size_t argslen = strlen(args);
	size_t tot = cmdlen + argslen + 4;
	if (tot > MAX_FTP_CMD_BUF) {
		fprintf(stderr, "[-] CMD size overflowed\n");
		exit(1);
	}
	char *send_buff = (char *)malloc(tot);
	snprintf(send_buff, tot, "%s %s\r\n", cmd, args);
	if (_debug_)
		fprintf(stdout, "[-->] %s", send_buff);
	send_message(send_buff);

	char recBUF[MAX_BUFF_SIZE];
	receive_message(recBUF);

	if (print_cmd)
		fprintf(stdout, "[<--] %s", recBUF);

	char resp[4];
	resp[3] = 0;
	resp[0] = recBUF[0];
	resp[1] = recBUF[1];
	resp[2] = recBUF[2];

	uint16_t resp_code = (uint16_t) atoi(resp);
	free(send_buff);

	return resp_code;
}

void send_username(const char *username)
{
	uint16_t resp = send_cmd("USER", username, 1);
}

int send_password(const char *password)
{
	uint16_t resp = send_cmd("PASS", password, 1);
	if (resp == LOGIN_SUCCESS)
		return 1;
	return 0;
}

void send_ciao()
{
	send_cmd("QUIT", "", 1);
}

void send_dir()
{
	int server_fd = create_data_channel("LIST", "", 1);
	if (server_fd <= 0) {
		fprintf(stdout, "[!] Operation failed [error_code = %d]\n",
			-server_fd);
		return;
	}
	save_into_file(server_fd, stdout);
}

void send_show(const char *file)
{
	int server_fd = create_data_channel("RETR", file, 1);
	if (server_fd <= 0) {
		fprintf(stdout, "[!] Operation failed [error_code = %d]\n",
			-server_fd);
		return;
	}
	save_into_file(server_fd, stdout);
}

int create_data_channel(const char *cmd, const char *args, int print_cmd)
{
	int servFD = -3453;
	if (_mode_ == ACTIVE) {
		int ret = open_act_connection();
		if (ret < 0)
			return -1;
		uint32_t addr =
		    ((uint32_t) 0xFFFFFFFF) & ((uint32_t) get_aaddr());
		uint16_t port = ((uint16_t) 0xFFFF) & ((uint16_t) get_aport());

		unsigned char *caddr = (unsigned char *)&addr;
		unsigned char *cport = (unsigned char *)&port;

		char buff[MAX_FTP_CMD_BUF] = { 0 };
		snprintf(buff, MAX_FTP_CMD_BUF, "%d,%d,%d,%d,%d,%d", caddr[0],
			 caddr[1], caddr[2], caddr[3], cport[0], cport[1]);
		send_cmd("PORT", buff, 1);
	} else if (_mode_ == PASSIVE) {
		servFD = open_pasv_connection();
		if (servFD < 0)
			return -1;
	}

	int rep = send_cmd(cmd, args, print_cmd);
	if (rep >= 400) {
		return -rep;
	}

	if (_mode_ == ACTIVE) {
		struct sockaddr_storage serverStorage;
		socklen_t addr_size = sizeof serverStorage;
		servFD =
		    accept(get_afd(), (struct sockaddr *)&serverStorage,
			   &addr_size);
	}
	return servFD;
}

int save_into_file(int fd, FILE * out)
{
	ssize_t n;
	char buff[MAX_BUFF_SIZE];

	int flags;
	if ((flags = fcntl(fd, F_GETFL, NULL)) < 0) {
		perror("[-] save_into_file");
		return -1;
	}
	flags |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flags) < 0) {
		return -1;
	}
	struct timeval tv;
	fd_set readfds;
	tv.tv_sec = 0;
	tv.tv_usec = 100000;
	FD_ZERO(&readfds);
	FD_SET(fd, &readfds);
	int rc;
	int ret;
	for (;;) {
		rc = select(fd + 1, &readfds, NULL, NULL, &tv);
		if (rc < 0) {
			perror("[-] select rc < 0");
			ret = -1;
			break;
		} else if (rc == 0) {
			ret = 1;
			break;
		} else {
			n = read(fd, buff, MAX_BUFF_SIZE - 1);
			if (n == 0) {
				ret = 1;
				break;
			} else if (n == -1) {
				perror("[-] select rc < 0");
				ret = -1;
				break;
			} else {
				fprintf(out, "%s", buff);
			}
		}
	}
	close(fd);
	if (out == stdout || out == stderr)
		fprintf(stdout, "\n");
	return ret;
}
