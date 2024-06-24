#include"server.h"
#include"sqlite3.h"

sqlite3* db;

// Function to handle SQLite errors
void handle_error(sqlite3* db) {
    fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
}

// Function to open SQLite connection
void open_database() {
    int rc = sqlite3_open("your_database.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
}

// Function to close SQLite connection
void close_database() {
    sqlite3_close(db);
}

// Function to add login details to SQLite database
void add_login_details(const char* userId, const char* password) {
    char sql[200];
    sprintf(sql, "INSERT INTO login_details (userId, password) VALUES ('%s', '%s');", userId, password);
    
    char* errMsg = 0;
    int rc = sqlite3_exec(db, sql, NULL, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    } else {
        printf("Inserted login details successfully.\n");
    }
}

// Function to add user data to SQLite database
void add_user_data(int client_number, const char* forwarding_number, int registered, int forward_activated, const char* forwarding_type) {
    char sql[300];
    sprintf(sql, "INSERT INTO user_data (client_number, forwarding_number, registered, forward_activated, forwarding_type) VALUES (%d, '%s', %d, %d, '%s');",
            client_number, forwarding_number, registered, forward_activated, forwarding_type);
    
    char* errMsg = 0;
    int rc = sqlite3_exec(db, sql, NULL, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    } else {
        printf("Inserted user data successfully.\n");
    }
}

// Function to delete login details from SQLite database by userId
void delete_login_details(const char* userId) {
    char sql[100];
    sprintf(sql, "DELETE FROM login_details WHERE userId = '%s';", userId);
    
    char* errMsg = 0;
    int rc = sqlite3_exec(db, sql, NULL, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    } else {
        printf("Deleted login details successfully.\n");
    }
}

// Function to delete user data from SQLite database by client_number
void delete_user_data(int client_number) {
    char sql[100];
    sprintf(sql, "DELETE FROM user_data WHERE client_number = %d;", client_number);
    
    char* errMsg = 0;
    int rc = sqlite3_exec(db, sql, NULL, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    } else {
        printf("Deleted user data successfully.\n");
    }
}

// Function to view all records from a table
void view_all(const char* table_name) {
    char sql[50];
    sprintf(sql, "SELECT * FROM %s;", table_name);
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        handle_error(db);
        return;
    }
    
    printf("Records from table %s:\n", table_name);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        // Retrieve values from the query
        // Example: int client_number = sqlite3_column_int(stmt, 0);
        // Example: const unsigned char* userId = sqlite3_column_text(stmt, 1);
        // Print or process the retrieved values as needed
    }
    
    sqlite3_finalize(stmt);
}

// Function to handle client requests
void handle_client(int client_socket) {
    char buffer[1024] = {0};
    int valread;
    
    valread = read(client_socket, buffer, 1024);
    if (valread <= 0) {
        fprintf(stderr, "Error reading from client.\n");
        return;
    }

    printf("%s\n", buffer);
    
    // Process the received command
    if (strcmp(buffer, "ADD_LOGIN") == 0) {
        // Example: add_login_details(db, "user1", "password1");
    } else if (strcmp(buffer, "ADD_USER") == 0) {
        // Example: add_user_data(db, 1, "1234567890", 1, 1, "Busy");
    } else if (strcmp(buffer, "DELETE_LOGIN") == 0) {
        // Example: delete_login_details(db, "user1");
    } else if (strcmp(buffer, "DELETE_USER") == 0) {
        // Example: delete_user_data(db, 1);
    } else if (strcmp(buffer, "VIEW_LOGIN") == 0) {
        // Example: view_all("login_details");
    } else if (strcmp(buffer, "VIEW_USER") == 0) {
        // Example: view_all("user_data");
    } else {
        fprintf(stderr, "Unknown command received from client: %s\n", buffer);
    }
    
    // Respond back to the client if needed
    // Example: send(client_socket, response_message, strlen(response_message), 0);
}

