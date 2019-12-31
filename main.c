#include <stdio.h>
#include "ftpc.h"
enum Mode _mode;
int _debug;
int main() {
    int debug = 0, passive = 0;
    char* host;
    uint16_t port;

    open_data_connection("127.0.0.1",21);
    char recbuff[MAX_BUFF_SIZE];
    receive_message(recbuff);
    printf("%s\n",recbuff);


    destroy_data_socket();

    return 0;
}