#ifndef __HEADER__
#define __HEADER__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sqlite3.h>

#define PORT 12345
#define MAX_CLIENTS 5
#define RESPONSE_SIZE 8192

extern void handle_error(sqlite3*);
extern void open_database();
extern void close_database();

extern void add_login_details(const char*, const char* password);
extern void add_user_data(const char*, const char*, int, int, int);
extern void delete_login_details(const char*);
extern void delete_user_data(const char*);

extern char* view_auth_table();
extern char* view_forwarding_table();
extern char* view_cfs_status(const char*);
extern char* view_cfs_code(const char*);

extern void update_forwarding_number(const char*, const char*);
extern void update_activation_status(const char*, int);
extern void update_forwarding_type(const char*, int);

extern void handle_client(int);

#endif
