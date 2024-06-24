#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define SERVER_ADDRESS "127.0.0.1"

extern int fill_command(char[], int);
extern int login(void);
extern int admin_menu(void);
