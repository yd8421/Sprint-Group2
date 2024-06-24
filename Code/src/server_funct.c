#include"server.h"
#include"sqlite3.h"

sqlite3* db;

// Used to display the results from the SQL SELECT query
int sql_select_callback(void *NotUsed, int argc, char **argv, char **azColName){
  int i;
  for(i=0; i<argc; i++){
    // printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    printf("%s\t", argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

// Function to handle SQLite errors
void handle_error(sqlite3* db) {
    fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
}

// Function to open SQLite connection
void open_database() {
    int rc = sqlite3_open("cfs_data.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }

    char *zErrMsg = 0;
    const char *init_table1 = "CREATE TABLE IF NOT EXISTS forwardinfo (clientNumber TEXT PRIMARY KEY,forwardNumber TEXT,isRegistered INTEGER,isActivated INTEGER,forwardType INTEGER);";
    rc = sqlite3_exec(db, init_table1, sql_select_callback, 0, &zErrMsg);
    if( rc ){
        // Log this section under ERROR
        fprintf(stderr, "Failed to create table: %s\n", sqlite3_errmsg(db));
    }

    const char *init_table2 = "CREATE TABLE IF NOT EXISTS authinfo (clientNumber TEXT PRIMARY KEY,passkey TEXT);";
    rc = sqlite3_exec(db, init_table2, sql_select_callback, 0, &zErrMsg);
    if( rc ){
        // Log this section under ERROR
        fprintf(stderr, "Failed to create table: %s\n", sqlite3_errmsg(db));
        sqlite3_free(zErrMsg);
        return;
    }
}

// Function to close SQLite connection
void close_database() {
    sqlite3_close(db);
}

// Function to add login details to SQLite database
void add_login_details(const char* userId, const char* password)
{
  char sql_query[256];
  char *zErrMsg = 0;
  int rc;
  sprintf(sql_query, "INSERT INTO authinfo VALUES (");
  sprintf(sql_query + strlen(sql_query), "'%s', ", userId);
  sprintf(sql_query + strlen(sql_query), "'%s'", password);
  sprintf(sql_query + strlen(sql_query), ");");
  // printf("[DEBUG] Prepared String: %s\n", sql_query);
  rc = sqlite3_exec(db, sql_query, sql_select_callback, 0, &zErrMsg);
  if( rc == SQLITE_CONSTRAINT){
    // Log this section under ERROR
    fprintf(stderr, "User '%s' is already registered to the CFS\n", userId);
    //fprintf(stderr, "Failed to execute query: %s\n", sqlite3_errmsg(db));
    sqlite3_free(zErrMsg);
    return;
  }
  if( rc ){
    // Log this section under ERROR
    fprintf(stderr, "Failed to execute query: %s\n", sqlite3_errmsg(db));
    sqlite3_free(zErrMsg);
    return;
  }
}

// Function to add user data to SQLite database
void add_user_data(const char* clientNumber, const char* forwardingNumber, int isRegistered, int isActivated, int forwardingType) {
    char sql_query[256];
    char *zErrMsg = 0;
    int rc;
    sprintf(sql_query, "INSERT INTO forwardinfo VALUES (");
    sprintf(sql_query + strlen(sql_query), "'%s', ", clientNumber);
    sprintf(sql_query + strlen(sql_query), "'%s', ", forwardingNumber);
    sprintf(sql_query + strlen(sql_query), "%d, ", isRegistered);
    sprintf(sql_query + strlen(sql_query), "%d, ", isActivated);
    sprintf(sql_query + strlen(sql_query), "%d", forwardingType);
    sprintf(sql_query + strlen(sql_query), ");");
    //printf("[DEBUG] Prepared String: %s\n", sql_query);
    rc = sqlite3_exec(db, sql_query, sql_select_callback, 0, &zErrMsg);
    if( rc == SQLITE_CONSTRAINT){
        // Log this section under ERROR
        fprintf(stderr, "User '%s' already exist!\n", clientNumber);
        //fprintf(stderr, "Failed to log user information, CODE %d: %s\n", rc, sqlite3_errmsg(db));
        sqlite3_free(zErrMsg);
        return;
    }
}

// Function to delete login details from SQLite database by userId
void delete_login_details(const char* userId) {
    char sql_query[100];
    sprintf(sql_query, "DELETE FROM login_details WHERE userId = '%s';", userId);
    
    char* errMsg = 0;
    int rc = sqlite3_exec(db, sql_query, NULL, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    } else {
        printf("Deleted login details successfully.\n");
    }
}

// Function to delete user data from SQLite database by client_number
void delete_user_data(const char* client_number) {
    char sql_query[100];
    sprintf(sql_query, "DELETE FROM user_data WHERE client_number ='%s';", client_number);
    
    char* errMsg = 0;
    int rc = sqlite3_exec(db, sql_query, NULL, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    } else {
        printf("Deleted user data successfully.\n");
    }
}

// Function to view all records from a table 'authinfo'
void view_auth_table() {
    char sql_query[50];
    sprintf(sql_query, "SELECT * FROM authinfo;");
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql_query, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        handle_error(db);
        return;
    }
    
    int tableHeaderStatus=0;
    
    printf("Client Number (User ID)\t| Password\t\n");
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        if(tableHeaderStatus == 0)
        {
            printf("Records from the authentication table:\n");
            printf("Client Number (User ID)\t| Password\t\n");
            printf("------------------------------------\t\n");
            tableHeaderStatus++;
        }
        printf("%s\t|%s\t\n", sqlite3_column_text(stmt, 0), sqlite3_column_text(stmt, 1));
    }
    
    sqlite3_finalize(stmt);
}

// Function to view all records from a table 'forwardinfo'
void view_forwarding_table() {
    char sql_query[50];
    sprintf(sql_query, "SELECT * FROM forwardinfo;");
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql_query, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        handle_error(db);
        return;
    }
    
    int tableHeaderStatus=0;
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        if(tableHeaderStatus == 0)
        {
            printf("Records from the forwarding table:\n");
            printf("Client Number \t| Forwarding Number \t| Registration \t| Activation \t| Forwarding Type\t\n");
            printf("---------------------------------------------------------------------------------------\t\n");
            tableHeaderStatus++;
        }
        printf("%s\t| ", sqlite3_column_text(stmt, 0));
        printf("%s\t| ", sqlite3_column_text(stmt, 1));
        printf("%s\t| ", sqlite3_column_text(stmt, 2));
        printf("%s\t| ", sqlite3_column_text(stmt, 3));
        printf("%s\t\n", sqlite3_column_text(stmt, 4));
    }
    
    sqlite3_finalize(stmt);
}

// Function to update forwarding number for a client
void update_forwarding_number(const char* clientNumber, const char* forwardingNumber){
    char sql_query[256];
    char *zErrMsg = 0;
    int rc;

    sprintf(sql_query, "UPDATE forwardinfo SET ");
    sprintf(sql_query + strlen(sql_query), "forwardNumber='%s'", forwardingNumber);
    sprintf(sql_query + strlen(sql_query), " WHERE clientNumber='%s';", clientNumber);
    //printf("[DEBUG] Prepared String: %s\n", sql_query);
    rc = sqlite3_exec(db, sql_query, sql_select_callback, 0, &zErrMsg);
    if( rc ){
        // Log this section under ERROR
        fprintf(stderr, "[ERROR] Failed to update forwarding number: %s\n", sqlite3_errmsg(db));
        sqlite3_free(zErrMsg);
        return;
    }
}

// Function to update call forwarding activation status for a client
void update_activation_status(const char* clientNumber, int isActivated)
{
    char sql_query[256];
    char *zErrMsg = 0;
    int rc;

    sprintf(sql_query, "UPDATE forwardinfo SET ");
    sprintf(sql_query + strlen(sql_query), "isActivated=%d", isActivated);
    sprintf(sql_query + strlen(sql_query), " WHERE clientNumber='%s';", clientNumber);
    //printf("[DEBUG] Prepared String: %s\n", sql_query);
    rc = sqlite3_exec(db, sql_query, sql_select_callback, 0, &zErrMsg);
    if( rc ){
        // Log this section under ERROR
        fprintf(stderr, "[ERROR] Failed to activate call forwarding: %s\n", sqlite3_errmsg(db));
        sqlite3_free(zErrMsg);
        return;
    }
}

// Function to update forwarding type for a client
void update_forwarding_type(const char* clientNumber, int forwardingType)
{
    char sql_query[256];
    char *zErrMsg = 0;
    int rc;

    sprintf(sql_query, "UPDATE forwardinfo SET ");
    sprintf(sql_query + strlen(sql_query), "forwardType=%d", forwardingType);
    sprintf(sql_query + strlen(sql_query), " WHERE clientNumber='%s';", clientNumber);
    //printf("[DEBUG] Prepared String: %s\n", sql_query);
    rc = sqlite3_exec(db, sql_query, sql_select_callback, 0, &zErrMsg);
    if( rc ){
        // Log this section under ERROR
        fprintf(stderr, "[ERROR] Failed to update forwarding type: %s\n", sqlite3_errmsg(db));
        sqlite3_free(zErrMsg);
        return;
    }
}

// Function to handle client requests
void handle_client(int client_socket) {
    char buffer[1024] = {0};
    char *token_params;
    int valread;
    
    valread = read(client_socket, buffer, 1024);
    if (valread <= 0) {
        fprintf(stderr, "[ERROR] Error reading from client.\n");
        return;
    }

    printf("[INFO] Buffer: %s\n", buffer);
    token_params = strtok(buffer, " ");

    // Process the received command
    if (strcmp(token_params, "ADD_LOGIN") == 0) {
        // Example: add_login_details(db, "user1", "password1");
        printf("[INFO] Request recieved to add login\n");
        const char* userId = strtok(buffer, " ");
        const char* password = strtok(buffer, " ");
        add_login_details(userId, password);
        
    } else if (strcmp(token_params, "ADD_USER") == 0) {
        printf("[INFO] Request recieved to add user\n");
        char* clientNumber = strtok(NULL, " ");
        char* forwardingNumber = strtok(NULL, " ");
        int isRegistered = atoi(strtok(NULL, " "));
        int isActivated = atoi(strtok(NULL, " "));
        int forwardType = 0;
        char* forwTypeBuffer = strtok(NULL, " ");
        //printf("DEBUG: forwType: %c %d\n", forwTypeBuffer[0], forwTypeBuffer[0]);
        if (forwTypeBuffer[0] == 'U') 
            forwardType = 1;
        if (forwTypeBuffer[0] == 'N')
            forwardType = 2;
        if (forwTypeBuffer[0] == 'B')
            forwardType = 3;

        // Example: add_user_data(db, 1, "1234567890", 1, 1, "Busy");
        add_user_data(clientNumber, forwardingNumber, isRegistered, isActivated, forwardType);

    } else if (strcmp(token_params, "DELETE_LOGIN") == 0) {
        // Example: delete_login_details(db, "user1");
        printf("[INFO] Request recieved to delete login\n");
        char* userId = strtok(NULL, " ");
        delete_login_details(userId);
        
    } else if (strcmp(token_params, "DELETE_USER") == 0) {
        printf("[INFO] Request recieved to delete user\n");
        char* userId = strtok(NULL, " ");

        // Example: delete_user_data(db, 1);
        delete_user_data(userId);
        
    } else if (strcmp(token_params, "VIEW_LOGIN") == 0) {
        printf("[INFO] Request recieved to view login data\n");
        view_auth_table();
    } else if (strcmp(token_params, "VIEW_USER") == 0) {
        printf("[INFO] Request recieved to view user forwarding data\n");
        view_forwarding_table();
    } else if (strcmp(token_params, "UPD_USER") == 0) {
        printf("[INFO] Request recieved to update user forwarding data\n");
        char* clientNumber = strtok(NULL, " ");
        char* forwardingNumber = strtok(NULL, " ");
        int isRegistered = atoi(strtok(NULL, " "));
        int isActivated = atoi(strtok(NULL, " "));
        int forwardType = 0;
        char* forwTypeBuffer = strtok(NULL, " ");
        //printf("DEBUG: forwType: %c %d\n", forwTypeBuffer[0], forwTypeBuffer[0]);
        if (forwTypeBuffer[0] == 'O')
            forwardType = -1;
        if (forwTypeBuffer[0] == 'U') 
            forwardType = 1;
        if (forwTypeBuffer[0] == 'N')
            forwardType = 2;
        if (forwTypeBuffer[0] == 'B')
            forwardType = 3;

        if(strcmp(forwardingNumber, "NA"))
        {
            update_forwarding_number(clientNumber,forwardingNumber);
            printf("[INFO] Updated forwarding number\n");
        }
        if(isActivated != -1)
        {
            update_activation_status(clientNumber,isActivated);
            printf("[INFO] Updated forwarding activation status\n");
        }
        if(forwardType != -1)
        {
            update_forwarding_type(clientNumber, forwardType);
            printf("[INFO] Updated forwarding type\n");
        }

    } else {
        fprintf(stderr, "[WARN] Unknown command received from client: %s\n", buffer);
    }

    // while(token_params != NULL)
    // {
    //     token_params = strtok(NULL, " ");
    // }
    
    // Respond back to the client if needed
    // Example: 
    char* response_message="[SERVER] Task completed\n";
    send(client_socket, response_message, strlen(response_message), 0);
}

