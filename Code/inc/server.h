#ifndef __HEADER__
#define __HEADER__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sqlite3.h>

#define PORT 12345
#define MAX_CLIENTS 5
#define RESPONSE_SIZE 8192
#define ENCRYPT_KEY 34

extern void interrupt_handler(int);

extern char* get_current_time();

extern char* encrypt_string(const char*);
extern char* decrypt_string(const char*);

extern void handle_error(sqlite3*);
extern int open_database();
extern void close_database();

extern char* add_login_details(const char*, const char* password);
extern char* add_user_data(const char*, const char*, int, int, int);
extern void delete_login_details(const char*);
extern void delete_user_data(const char*);

extern char* view_auth_table();
extern char* view_forwarding_table();
extern char* validate_auth_info(const char*, const char*);
extern char* view_cfs_status(const char*);
extern char* view_cfs_code(const char*);

extern int update_forwarding_number(const char*, const char*);
extern int update_registration_status(const char*, int);
extern int update_activation_status(const char*, int);
extern int update_forwarding_type(const char*, int);

extern int handle_client(int, const char*);

#endif
