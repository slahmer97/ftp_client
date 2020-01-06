#include <stdio.h>
#include "ftpc.h"
#include <stdlib.h>
#include "glob.h"
enum Mode _mode_;
int _debug_;
enum STATUS _status_;
char current_user[250] = { 0 };

int running = 1;
int try_count = 0;

void cmd_handler(const char *cmd);

/*
* Le main !
*/
int main(int argc, char *argv[])
{
	init_das();
	login(0);
	debugf(0);		// debug mode in off mode
	passivef(0);		// active mode by default
	int with_cli_arg = 0;
	if (argc > 1)
		with_cli_arg = 1;
	while (running) {
		char buff[MAX_FTP_CMD_BUF] = { 0 };
		if (_status_ == CONNECTED)
			fprintf(stdout, "%s@Ftp> ", current_user);
		else {
			if (with_cli_arg) {		// takes entry from cli arguments
				fprintf(stdout, "Ftp> open %s\n", argv[1]);
				strcat(strcat(strcat(buff, "open "), argv[1]), "\n");
				with_cli_arg = 0;
				goto with_cli;
			} else {
				fprintf(stdout, "Ftp> ");
			}
		}
		fgets(buff, sizeof(buff), stdin);
 with_cli:
		if (buff[0] == '\n')
			continue;
		if(strncmp(buff,"clear",5) == 0){
		    system("clear");
            continue;
		}
		cmd_handler(buff);
	}
	destroy_data_socket();

	return 0;
}

/*
* A chaque nouvelle commande entree par l'utilisateur,
* cette fonction est appelee pour la geree.
*/
void cmd_handler(const char *__cmd)
{
	char *tmp = (char *)__cmd;
	while (*tmp && *(tmp++) != '\n') ;
	*(tmp - 1) = 0;

	char *command = (char *)__cmd;
	char *param;
	enum FTP_C_CMD cmd = get_command(command);	// parse command string
	switch (cmd) {
	case OPEN:
		if (_status_ == CONNECTED)
			goto already_logged_in;

		param = command + 5;
		char username[250] = { 0 };
		char password[250] = { 0 };
		char recbuff[MAX_BUFF_SIZE] = { 0 };
		open_data_connection(getsockaddr(), param, 21);
		receive_message(recbuff);
		passivef(0);
 retry:	;
		try_count++;
		fprintf(stdout, "Username : ");
		fgets(username, sizeof(username), stdin);	// read string
		int pasw_dmd = send_username(username);
		if (pasw_dmd != PASSWORD_DEMAND) {
			fprintf(stdout, "[-] Rejected Message,\n");
		}
		fprintf(stdout, "Password : ");
		fgets(password, sizeof(password), stdin);	// read string
		int is_connect = send_password(password);
		if (is_connect == 0) {
			fprintf(stdout, "[!] Login failed\n");
			if (try_count == 2) {
				fprintf(stderr, "[-] Try count exceeded\n");
                return;
			}
			goto retry;
		}

		{
			tmp = (char *)username;
			while (*tmp && *(tmp++) != '\n') ;
			*(tmp - 1) = 0;
		}
		snprintf(current_user, 250, "%s", username);
		login(1);
		break;
	case DIR:
		if (_status_ == DISCONNECTED)
			goto login_required;
		send_dir();
		break;
	case SHOW:
		if (_status_ == DISCONNECTED)
			goto login_required;
		param = command + 5;
		//TODO check if valid file name!
		send_show((const char *)param);
		break;
	case CIAO:
		if (_status_ == DISCONNECTED) {
			fprintf(stdout,
				"[!] you do not have an active connection.\n");
			break;
		}
		send_ciao();
		memset(current_user, 0, sizeof(current_user));
		login(0);
		break;
	case EXIT:
		if (_status_ == CONNECTED)
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
		passivef(1);
		break;
	case PASSIVE_OFF:
		passivef(0);
		break;
	case GET_FILE:
		if (_status_ == DISCONNECTED)
			goto login_required;
		param = command + 4;
		get_file(param);
		break;
	case SEND_FILE:
		if (_status_ == DISCONNECTED)
			goto login_required;
		break;
	case REN_FILE:
		if (_status_ == DISCONNECTED)
			goto login_required;

		break;
	case DEL_FILE:
		if (_status_ == DISCONNECTED)
			goto login_required;

		break;
	case CD:
		if (_status_ == DISCONNECTED)
			goto login_required;
		param = command + 3;
		send_cd(param);
		break;
	case PWD:
		if (_status_ == DISCONNECTED)
			goto login_required;
		send_pwd();
		break;
	case MKDIR:
		if (_status_ == DISCONNECTED)
			goto login_required;
		param = command + 4;
		send_mkdir(param);
		break;
	case RMDIR:
		if (_status_ == DISCONNECTED)
			goto login_required;
		param = command + 4;
		send_rmdir(param);
		break;
	case INVALID_CMD:
	default:
		fprintf(stdout, "[-] Invalid command\n");
		break;
	case HISTORY:
		//TODO
		break;
	case HELP:
		show_help();
		break;
	case ASCII:
		if (_status_ == DISCONNECTED)
			goto login_required;
		send_ascii();
		break;
	case BINARY:
		if (_status_ == DISCONNECTED)
			goto login_required;
		send_binary();
		break;
	}
	return;
 login_required:
	fprintf(stdout, "[-] this operation requires valid connection. \n");
	return;

 already_logged_in:
	fprintf(stdout,
		"[-] you have already an opened connection, if you wish to open new, please close the old one [Ciao] \n");
}
