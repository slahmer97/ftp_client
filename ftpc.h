//
// Created by slahmer on 12/31/19.
//

#ifndef FTP_CLIENT_FTPC_H
#define FTP_CLIENT_FTPC_H

#include "ftpsock.h"



enum Mode {
    ACTIVE,
    PASSIVE,
};
enum Command {
    LS,
    PUT,
    MPUT,
    GET,
    MGET,
    CD,
    LCD,
    DELE,
    MDELE,
    MKDIR,
    RMDIR,
    PWD,
    EXIT,
    PASV,
    INVALID_COMMAND,
};

extern enum Mode _mode;
extern int _debug;
void passive(int flag);
void debug(int flag);



#endif //FTP_CLIENT_FTPC_H
