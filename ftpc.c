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
    else if(strncmp("hist",cmd,4) == 0)
        return HISTORY;
    else if(strncmp("help",cmd,4) == 0)
        return HELP;
    return INVALID_CMD;

}
void passivef(int flag){ _mode_ = flag ? PASSIVE : ACTIVE; }
void debugf(int flag) { _debug_ = flag; }
void login(int flag){_status_ = flag ? CONNECTED : DISCONNECTED;}

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
    if(_debug_)
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
void send_ciao(){
    send_cmd("QUIT","",1);
}
void send_dir(){

}


void create_data_channel(const char* line){
    if(_mode_ == PASSIVE){
        int ret = open_passive_connection();
        if(ret < 0)
            return;
        uint32_t addr =((uint32_t)0xFFFFFFFF)&((uint32_t)get_aaddr());
        uint16_t port =((uint16_t)0xFFFF)&((uint16_t)get_aport());

        unsigned char* caddr = (unsigned char*)&addr;
        unsigned char* cport = (unsigned char*)&port;

        char buff[MAX_FTP_CMD_BUF] = {0};
        snprintf(buff,MAX_FTP_CMD_BUF,"%d,%d,%d,%d,%d,%d",caddr[0],caddr[1],caddr[2],caddr[3],cport[0],cport[1]);
        send_cmd("PORT",buff,1);
    }
    if(_mode_ == ACTIVE){


    }
}