#ifndef __HEADER__
#define __HEADER__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 4096

extern int main_menu();
extern void register_user(char[]);
extern void register_user_pass(char[]);
extern int login();
extern void call(char[]);
extern void myflush(void);
extern int user_menu(void);
extern void update_client_data(void);
extern void save(char[]);

#endif
