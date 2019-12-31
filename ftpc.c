//
// Created by slahmer on 12/31/19.
//
#include "ftpc.h"
#include <stdlib.h>
#include <stdio.h>

enum FTP_C_CMD get_command(const char* cmd){
    if(strncmp("open",cmd,4) == 0)
        return OPEN;
    else if(strncmp("dir",cmd,3) == 0)
        return DIR;
    else if(strncmp("show",cmd,4) == 0)
        return SHOW;
    else if(strncmp("ciao",cmd,4) == 0)
        return CIAO;
    else if(strncmp("exit",cmd,4) == 0)
        return EXIT;
    else if(strncmp("debugon",cmd,7) == 0)
        return DEBUG_ON;
    else if(strncmp("debugoff",cmd,8) == 0)
        return DEBUG_OFF;
    else if(strncmp("passiveon",cmd,9) == 0)
        return PASSIVE_ON;
    else if(strncmp("passiveoff",cmd,10) == 0)
        return PASSIVE_OFF;
    else if(strncmp("get",cmd,3) == 0)
        return GET_FILE;
    else if(strncmp("send",cmd,4) == 0)
        return SEND_FILE;
    else if(strncmp("ren",cmd,3) == 0)
        return REN_FILE;
    else if(strncmp("del",cmd,3) == 0)
        return DEL_FILE;
    else if(strncmp("cd",cmd,2) == 0)
        return CD;
    else if(strncmp("mdir",cmd,4) == 0)
        return MKDIR;
    else if(strncmp("rdir",cmd,4) == 0)
        return RMDIR;
    return INVALID_CMD;

}
void passivef(int flag){ _mode = flag ? PASSIVE : ACTIVE; }
void debugf(int flag) { _debug = flag; }

uint16_t send_cmd(const char*cmd,const char*args,int print_cmd){
    size_t cmdlen = strlen(cmd);
    size_t argslen = strlen(args);
    size_t tot = cmdlen+argslen+4;
    if (tot > MAX_FTP_CMD_BUF) {
        fprintf(stderr,"[-] CMD size overflowed\n");
        exit(1);
    }
    char* send_buff = (char*)malloc(tot);
    snprintf(send_buff,tot,"%s %s\r\n",cmd,args);
    if(_debug)
        fprintf(stdout,"[-->] %s",send_buff);
    send_message(send_buff);

    char recBUF[MAX_BUFF_SIZE];
    receive_message(recBUF);

    if(print_cmd)
        fprintf(stdout,"[<--] %s",recBUF);

    char resp[4];resp[3]=0;resp[0]=recBUF[0];resp[1]=recBUF[1];resp[2]=recBUF[2];

    uint16_t resp_code =(uint16_t) atoi(resp);
    free(send_buff);

    return resp_code;
}

void send_username(const char*username){
    uint16_t resp = send_cmd("USER",username,1);
}
int send_password(const char*password){
    uint16_t resp = send_cmd("PASS",password,1);
    if(resp == LOGIN_SUCCESS)
        return 1;
    return 0;
}
