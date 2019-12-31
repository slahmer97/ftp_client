//
// Created by slahmer on 12/31/19.
//

#ifndef FTP_CLIENT_FTPC_H
#define FTP_CLIENT_FTPC_H
#define MAX_FTP_CMD_BUF 500

#include "ftpsock.h"
/*
 * check wiki : https://en.wikipedia.org/wiki/List_of_FTP_server_return_codes
 */
enum FTP_RESP_CODE {
    PASSWORD_DEMAND=331,
    LOGIN_SUCCESS = 230,
    CLOSING_DATA_CON = 226,
    FILE_STATUS_OKAY = 150,
    ENTERING_PASSIVE_MODE = 227,
};
enum FTP_C_CMD{
    OPEN,
    DIR,
    SHOW,
    CIAO,
    EXIT,
    DEBUG_ON,
    DEBUG_OFF,
    PASSIVE_ON,
    PASSIVE_OFF,
    GET_FILE,
    SEND_FILE,
    REN_FILE,
    DEL_FILE,
    CD,
    MKDIR,
    RMDIR,
    INVALID_CMD
};

enum Mode {
    ACTIVE,
    PASSIVE,
};


extern enum Mode _mode;
extern int _debug;
void passivef(int flag);
void debugf(int flag);



enum FTP_C_CMD get_command(const char*);

uint16_t send_cmd(const char*cmd,const char*args,int print_cmd);
void send_username(const char*);
int send_password(const char*);



#endif //FTP_CLIENT_FTPC_H
