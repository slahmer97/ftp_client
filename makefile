ftp_client: main.c ftpc.h ftpc.c ftpsock.h ftpsock.c
	gcc -o ftp_client main.c ftpc.c ftpsock.c -Wall

