cmake_minimum_required(VERSION 3.15)
project(ftp_client C)

set(CMAKE_C_STANDARD 99)

add_executable(ftp_client main.c ftpc.c ftpc.h ftpsock.h ftpsock.c)