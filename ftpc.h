//
// Created by slahmer on 12/31/19.
//

#ifndef FTP_CLIENT_FTPC_H
#define FTP_CLIENT_FTPC_H
#define MAX_FTP_CMD_BUF 500

#include <bits/types/FILE.h>
#include "ftpsock.h"
/*
 * check wiki : https://en.wikipedia.org/wiki/List_of_FTP_server_return_codes
 */
enum FTP_RESP_CODE {
	PASSWORD_DEMAND = 331,
	LOGIN_SUCCESS = 230,
	CLOSING_DATA_CON = 226,
	FILE_STATUS_OKAY = 150,
	ENTERING_PASSIVE_MODE = 227,
};
enum FTP_C_CMD {
	OPEN,			    //DONE
	DIR,                //DONE
	SHOW,               //DONE
	CIAO,			    //DONE
	EXIT,			    //DONE
	DEBUG_ON,		    //DONE
	DEBUG_OFF,		    //DONE
	PASSIVE_ON,         // DONE
	PASSIVE_OFF,        //DONE
	GET_FILE,           //DONE TODO check..
	SEND_FILE,
	REN_FILE,
	DEL_FILE,
	CD,
	MKDIR,
	RMDIR,
	INVALID_CMD,		//DONE
	HISTORY,
	HELP,
	BINARY,             //DONE
	ASCII               //DONE
};

enum Mode {
	ACTIVE,
	PASSIVE,
};
enum STATUS {
	CONNECTED,
	DISCONNECTED,
};

extern enum STATUS _status_;
extern enum Mode _mode_;
extern int _debug_;

void login(int flag);
void passivef(int flag);
void debugf(int flag);

enum FTP_C_CMD get_command(const char *);

uint16_t send_binary();
uint16_t send_ascii();
uint16_t send_cmd(const char *cmd, const char *args, int print_cmd);
uint16_t send_username(const char *);
int send_password(const char *);
void send_ciao();
void send_dir();
void send_show(const char *);
void get_file(const char*);

int create_data_channel(const char *cmd, const char *args, int print_cmd);

int save_into_file(int fd_source, FILE * out);

#endif				//FTP_CLIENT_FTPC_H
