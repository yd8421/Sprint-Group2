#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sqlite3.h>

#define PORT 12345
#define MAX_CLIENTS 5

extern void handle_error(sqlite3*);
extern void open_database();
extern void close_database();

extern void add_login_details(const char*, const char* password);
extern void add_user_data(int, const char*, int, int, const char*);
extern void delete_login_details(const char*);
extern void delete_user_data(int);

extern void view_all(const char* table_name);

extern void handle_client(int);
