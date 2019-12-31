#include <stdio.h>
#include "ftpc.h"
#include <stdlib.h>
#include <stdio.h>
enum Mode _mode;
int _debug;
enum STATUS status;
char current_user[250] ={0};
int running=1;
int try_count = 0;

void cmd_handler(const char*cmd);
int main() {
    status = DISCONNECTED;



    while (running){
        char buff[MAX_FTP_CMD_BUF] = {0};
        if(status == CONNECTED)
            fprintf(stdout,"%s@Ftp> ",current_user);
        else
            fprintf(stdout,"Ftp> ");
        fgets(buff,sizeof(buff),stdin);
        cmd_handler(buff);
    }






    destroy_data_socket();

    return 0;
}
void cmd_handler(const char*__cmd){
    char* tmp =(char*) __cmd;
    while (*tmp && *(tmp++) != '\n');
    *(tmp-1) = 0;

    char* command = (char*)__cmd;
    char* param;
    enum FTP_C_CMD cmd = get_command(command);// parse command string
    switch (cmd){
        case OPEN:
            // TODO check if not already logged in
            param = command+5;
            char username[250] = {0};
            char password[250] = {0};
            char recbuff[MAX_BUFF_SIZE] = {0};
            open_data_connection(param,21);
            receive_message(recbuff);
            passivef(0);
            debugf(1);
        retry:;
            try_count++;
            fprintf(stdout,"Username : ");
            fgets(username, sizeof(username), stdin);  // read string
            send_username(username);
            fprintf(stdout,"Password : ");
            fgets(password, sizeof(password), stdin);  // read string
            int is_connect = send_password(password);
            if(is_connect == 0){
                fprintf(stdout,"[!] Login failed\n");
                if(try_count == 2){
                    fprintf(stderr,"[-] Try count exceeded\n");
                    exit(EXIT_FAILURE);
                }
                goto retry;
            }
            status = CONNECTED;
            break;
        case DIR:
            break;
        case SHOW:
            break;
        case CIAO:
            //TODO shutdown the TCP stream
            send_ciao();
            memset(current_user,0,sizeof(current_user));
            status = DISCONNECTED;
            break;
        case EXIT:
            if(status == CONNECTED)
                send_ciao();
            running = 0;
            break;
        case DEBUG_ON:
            debugf(1);
            break;
        case DEBUG_OFF:
            debugf(0);
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
            fprintf(stdout,"[-] Invalid command\n");
            break;
    }



}