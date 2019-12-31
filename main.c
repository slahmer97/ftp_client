#include <stdio.h>
#include "ftpc.h"
#include <stdlib.h>
enum Mode _mode;
int _debug;
void cmd_handler(const char*cmd);
int main() {
    int try_count = 0;
    int debug = 1, passive = 0;
    char* host;
    uint16_t port;

    open_data_connection("127.0.0.1",21);
    char recbuff[MAX_BUFF_SIZE];
    receive_message(recbuff);
    passivef(passive);
    debugf(debug);
    printf("%s\n",recbuff);

    retry:;
    try_count++;
    char* username = "";
    send_username(username);
    char* password = "";
    int is_connect = send_password(password);
    if(is_connect == 0){
        fprintf(stdout,"[!] Login failed\n");
        if(try_count == 10){
            fprintf(stderr,"[-] Try count exceeded\n");
            exit(EXIT_FAILURE);
        }
        goto retry;
    }






    destroy_data_socket();

    return 0;
}
void cmd_handler(const char*__cmd){

    char* command = (char*)__cmd;
    char* param;


    enum FTP_C_CMD cmd = get_command(command);// parse command string
    switch (cmd){
        case OPEN:
            break;
        case DIR:
            break;
        case SHOW:
            break;
        case CIAO:
            break;
        case EXIT:
            break;
        case DEBUG_ON:
            break;
        case DEBUG_OFF:
            break;
        case PASSIVE_ON:
            break;
        case PASSIVE_OFF:
            break;
        case GET_FILE:
            break;
        case SEND_FILE:
            break;
        case REN_FILE:
            break;
        case DEL_FILE:
            break;
        case CD:
            break;
        case MKDIR:
            break;
        case RMDIR:
            break;
        case INVALID_CMD:
        default:

            break;
    }



}