#include"server.h"
#include"sqlite3.h"

sqlite3* g_db;

// Used to get the current time
char* get_current_time() {
    time_t rawTime;
    struct tm *timeInfo;
    char *buffer;

    time(&rawTime);
    timeInfo = localtime(&rawTime);

    buffer = (char*)malloc(20 * sizeof(char));
    if (buffer == NULL) {
        perror("Failed to allocate memory");
        return NULL;
    }

    strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", timeInfo);

    return buffer;
}

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
void handle_error(sqlite3* g_db) {
    fprintf(stderr, "[ERROR] SQLite error: %s\n", sqlite3_errmsg(g_db));
    sqlite3_close(g_db);
}

// Function to open SQLite connection
void open_database() {
    int returnCode = sqlite3_open("cfs_data.db", &g_db);
    if (returnCode != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(g_db));
        sqlite3_close(g_db);
        exit(1);
    }

    char *zErrMsg = 0;
    const char *init_table1 = "CREATE TABLE IF NOT EXISTS forwardinfo (clientNumber TEXT PRIMARY KEY,forwardNumber TEXT,isRegistered INTEGER,isActivated INTEGER,forwardType INTEGER);";
    returnCode = sqlite3_exec(g_db, init_table1, sql_select_callback, 0, &zErrMsg);
    if( returnCode ){
        // Log this section under ERROR
        fprintf(stderr, "[ERROR] Failed to create table: %s\n", sqlite3_errmsg(g_db));
    }

    const char *init_table2 = "CREATE TABLE IF NOT EXISTS authinfo (clientNumber TEXT PRIMARY KEY,passkey TEXT);";
    returnCode = sqlite3_exec(g_db, init_table2, sql_select_callback, 0, &zErrMsg);
    if( returnCode ){
        // Log this section under ERROR
        fprintf(stderr, "[ERROR] Failed to create table: %s\n", sqlite3_errmsg(g_db));
        sqlite3_free(zErrMsg);
        return;
    }
}

// Function to close SQLite connection
void close_database() {
    sqlite3_close(g_db);
}

// Function to add login details to SQLite database
void add_login_details(const char* userId, const char* password)
{
  char sql_query[256];
  char *zErrMsg = 0;
  int returnCode;
  sprintf(sql_query, "INSERT INTO authinfo VALUES (");
  sprintf(sql_query + strlen(sql_query), "'%s', ", userId);
  sprintf(sql_query + strlen(sql_query), "'%s'", password);
  sprintf(sql_query + strlen(sql_query), ");");
  // printf("[DEBUG] Prepared String: %s\n", sql_query);
  returnCode = sqlite3_exec(g_db, sql_query, sql_select_callback, 0, &zErrMsg);
  if( returnCode == SQLITE_CONSTRAINT){
    // Log this section under ERROR
    fprintf(stderr, "[ERROR] User '%s' is already registered to the CFS\n", userId);
    //fprintf(stderr, "Failed to execute query: %s\n", sqlite3_errmsg(g_db));
    sqlite3_free(zErrMsg);
    return;
  }
  if( returnCode ){
    // Log this section under ERROR
    fprintf(stderr, "Failed to execute query: %s\n", sqlite3_errmsg(g_db));
    sqlite3_free(zErrMsg);
    return;
  }
}

// Function to add user data to SQLite database
void add_user_data(const char* clientNumber, const char* forwardingNumber, int isRegistered, int isActivated, int forwardingType) {
    char sql_query[256];
    char *zErrMsg = 0;
    int returnCode;
    sprintf(sql_query, "INSERT INTO forwardinfo VALUES (");
    sprintf(sql_query + strlen(sql_query), "'%s', ", clientNumber);
    sprintf(sql_query + strlen(sql_query), "'%s', ", forwardingNumber);
    sprintf(sql_query + strlen(sql_query), "%d, ", isRegistered);
    sprintf(sql_query + strlen(sql_query), "%d, ", isActivated);
    sprintf(sql_query + strlen(sql_query), "%d", forwardingType);
    sprintf(sql_query + strlen(sql_query), ");");
    //printf("[DEBUG] Prepared String: %s\n", sql_query);
    returnCode = sqlite3_exec(g_db, sql_query, sql_select_callback, 0, &zErrMsg);
    if( returnCode == SQLITE_CONSTRAINT){
        // Log this section under ERROR
        fprintf(stderr, "User '%s' already exist!\n", clientNumber);
        //fprintf(stderr, "Failed to log user information, CODE %d: %s\n", returnCode, sqlite3_errmsg(g_db));
        sqlite3_free(zErrMsg);
        return;
    }
}

// Function to delete login details from SQLite database by userId
void delete_login_details(const char* userId) {
    char sql_query[100];
    sprintf(sql_query, "DELETE FROM authinfo WHERE clientNumber = '%s';", userId);
    
    char* errMsg = 0;
    int returnCode = sqlite3_exec(g_db, sql_query, NULL, 0, &errMsg);
    if (returnCode != SQLITE_OK) {
        fprintf(stderr, "[ERROR] SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    } else {
        printf("[INFO] Deleted login details successfully.\n");
    }
}

// Function to delete user data from SQLite database by client_number
void delete_user_data(const char* client_number) {
    char sql_query[100];
    sprintf(sql_query, "DELETE FROM forwardinfo WHERE clientNumber ='%s';", client_number);
    
    char* errMsg = 0;
    int returnCode = sqlite3_exec(g_db, sql_query, NULL, 0, &errMsg);
    if (returnCode != SQLITE_OK) {
        fprintf(stderr, "[ERROR] SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    } else {
        printf("[INFO] Deleted user data successfully.\n");
    }
}

// Function to view all records from a table 'authinfo'
char* view_auth_table() {
    char sql_query[50];
    char* errMsg = 0;
    char* response_data = (char*)malloc(RESPONSE_SIZE * sizeof(char));
    sprintf(sql_query, "SELECT * FROM authinfo;");
    
    sqlite3_stmt* stmt;
    int returnCode = sqlite3_prepare_v2(g_db, sql_query, -1, &stmt, NULL);
    if (returnCode != SQLITE_OK) {
        fprintf(stderr, "[ERROR] SQL error: %s\n", errMsg);
        sprintf(response_data, "[SERVER] SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return response_data;
    }
    
    int tableHeaderStatus=0;
    
    while ((returnCode = sqlite3_step(stmt)) == SQLITE_ROW) {
        if(tableHeaderStatus == 0)
        {
            printf("[INFO] Records from the authentication table:\n");
            //printf("Client Number (User ID)\t| Password\t\n");
            printf("%-10s | %-10s\n", "Client Number", "Password");
            printf("----------------------------------\n");

            sprintf(response_data, "[SERVER] Records from the authentication table:\n");
            sprintf(response_data + strlen(response_data), "%-10s | %-10s\n", "Client Number", "Password");
            sprintf(response_data + strlen(response_data), "----------------------------------\n");
            tableHeaderStatus++;
        }
        printf("%-13s | %-12s\n", sqlite3_column_text(stmt, 0), sqlite3_column_text(stmt, 1));
        sprintf(response_data + strlen(response_data), "%-13s | %-12s\n", sqlite3_column_text(stmt, 0), sqlite3_column_text(stmt, 1));
    }
    
    sqlite3_finalize(stmt);
    return response_data;
}

// Function to view all records from a table 'forwardinfo'
char* view_forwarding_table() {
    char sql_query[50];
    char* response_data = (char*)malloc(RESPONSE_SIZE * sizeof(char));
    char* errMsg = 0;
    sprintf(sql_query, "SELECT * FROM forwardinfo;");
    
    sqlite3_stmt* stmt;
    int returnCode = sqlite3_prepare_v2(g_db, sql_query, -1, &stmt, NULL);
    if (returnCode != SQLITE_OK) {
        fprintf(stderr, "[ERROR] SQL error: %s\n", errMsg);
        sprintf(response_data, "[SERVER] SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return response_data;
    }
    
    int tableHeaderStatus=0;
    
    while ((returnCode = sqlite3_step(stmt)) == SQLITE_ROW) {
        if(tableHeaderStatus == 0)
        {
            printf("[INFO] Records from the forwarding table:\n");
            printf("%-10s | %-10s | %-5s | %-5s | %-5s\n", "Client Number", "Forw. Number", "Reg. Status", "Act. Status", "Forw. Type");
            //printf("Client Number \t| Forwarding Number \t| Registration \t| Activation \t| Forwarding Type\t\n");
            printf("-----------------------------------------------------------------------\n");

            sprintf(response_data, "[SERVER] Records from the forwarding table:\n");
            sprintf(response_data + strlen(response_data), "%-10s | %-10s | %-5s | %-5s | %-5s\n", "Client Number", "Forw. Number", "Reg. Status", "Act. Status", "Forw. Type");
            sprintf(response_data + strlen(response_data), "-----------------------------------------------------------------------\n");
            tableHeaderStatus++;
        }
        printf("%-13s | ", sqlite3_column_text(stmt, 0));
        printf("%-12s | ", sqlite3_column_text(stmt, 1));
        printf("%-11s | ", sqlite3_column_text(stmt, 2));
        printf("%-11s | ", sqlite3_column_text(stmt, 3));
        printf("%-9s\n", sqlite3_column_text(stmt, 4));

        sprintf(response_data + strlen(response_data), "%-13s | ", sqlite3_column_text(stmt, 0));
        sprintf(response_data + strlen(response_data), "%-12s | ", sqlite3_column_text(stmt, 1));
        sprintf(response_data + strlen(response_data), "%-11s | ", sqlite3_column_text(stmt, 2));
        sprintf(response_data + strlen(response_data), "%-11s | ", sqlite3_column_text(stmt, 3));
        sprintf(response_data + strlen(response_data), "%-9s\n", sqlite3_column_text(stmt, 4));
    }
    
    if (tableHeaderStatus == 0){
        printf("[INFO] There are no records in the forwarding table\n");
        sprintf(response_data, "[SERVER] There are no records in the forwarding table\n");
    }

    sqlite3_finalize(stmt);
    return response_data;
}

// Function to view the call forwarding record of a particular client
char* view_cfs_status(const char* clientNumber)
{
    char* responseData = (char*)malloc(RESPONSE_SIZE * sizeof(char));
    char sql_query[256];
    char forwardNumber[11];
    char *errMsg = 0;
    int returnCode;

    sqlite3_stmt *res;

    sprintf(sql_query, "SELECT isRegistered, isActivated, forwardType FROM forwardinfo");
    sprintf(sql_query + strlen(sql_query), " WHERE clientNumber='%s';", clientNumber);

    //printf("[DEBUG] Prepared String: %s\n", sql_query);
    if (sqlite3_prepare_v2(g_db, sql_query, -1, &res, NULL) != SQLITE_OK) {
        printf("[ERROR] Can't retrieve data: %s\n", sqlite3_errmsg(g_db));
        sprintf(responseData, "[SERVER] SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return responseData;
    }

    // Status flag for CFS
    // 0 - None
    // 1 - Registered
    // 2 - Registered and Activated
    int statusFlag=0;

    // For forwarding type
    // 0  CF_NOTSET
    // 1  CF_UNCONDITIONAL
    // 2  CF_BUSY
    // 3  CF_NOREPLY
    int forwType=0;

    while (sqlite3_step(res) == SQLITE_ROW) {
        statusFlag+=atoi(sqlite3_column_text(res, 0));
        statusFlag+=atoi(sqlite3_column_text(res, 1));
        forwType=atoi(sqlite3_column_text(res, 2));
    }
    //printf("Statusflag: %d\n", statusFlag);

    switch(statusFlag)
    {
    case 0: printf("[INFO] User has not registered for CFS\n");
            sprintf(responseData, "[SERVER] User has not registered for CFS\n");
            break;
    case 1: printf("[INFO] User has not activated CFS\n");
            sprintf(responseData, "[SERVER] User has not activated CFS\n");
            break;
    case 2: printf("[INFO] User has activated CFS\n");
            sprintf(responseData, "[SERVER] User has activated CFS\n");
            break;
    }

    if(statusFlag != 2)
    {
        printf("[INFO] Call will be connected to %s\n", clientNumber);
        sprintf(responseData + strlen(responseData), "[SERVER] Call will be connected to %s\n", clientNumber);
        return responseData;
    }

    sprintf(sql_query, "SELECT forwardNumber FROM forwardinfo");
    sprintf(sql_query + strlen(sql_query), " WHERE clientNumber='%s';", clientNumber);

    //printf("[DEBUG] Prepared String: %s\n", sql_query);
    if (sqlite3_prepare_v2(g_db, sql_query, -1, &res, NULL) != SQLITE_OK) {
        printf("[ERROR] Can't retrieve data: %s\n", sqlite3_errmsg(g_db));
        sprintf(responseData, "[SERVER] SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return responseData;
    }

    while (sqlite3_step(res) == SQLITE_ROW) {
    strcpy(forwardNumber,sqlite3_column_text(res, 0));
    }
    switch(forwType)
    {
    case 0: printf("[INFO] Forward Type: Not Set\n");
            printf("[INFO] Call will be connected to %s\n", clientNumber);

            sprintf(responseData + strlen(responseData), "[SERVER] Forward Type: Not Set\n");
            sprintf(responseData + strlen(responseData), "[SERVER] Call will be connected to %s\n", clientNumber);
            return responseData;
            break;
    case 1: printf("[INFO] Forward Type: Unconditional\n");
            sprintf(responseData + strlen(responseData), "[SERVER] Forward Type: Unconditional\n");
            break;
    case 2: printf("[INFO] Forward Type: Busy\n");
            sprintf(responseData + strlen(responseData), "[SERVER] Forward Type: Busy\n");
            break;
    case 3: printf("[INFO] Forward Type: No Reply\n");
            sprintf(responseData + strlen(responseData), "[SERVER] Forward Type: No Reply\n");
            break;
    }
    printf("[INFO] Call will be forwarded to %s\n", forwardNumber);
    sprintf(responseData + strlen(responseData), "[SERVER] Call will be forwarded to %s\n", forwardNumber);

    return responseData;
}

// For Client: Function to fetch the call forwarding record of a particular client
//             and send it back to the client to simulate a phone call
// Expected return values for client:
// ERROR - An error occured while processing the request.
// NF 900000000 - Direct the call to the client number.
//                (No forwarding rules / Forwarding not activated / Forwarding number not set)
// F 9900121211 1/2/3 - Call forwarding enabled.
//                      Direct the call to the forwarding number and simulate the call as per the forwarding type
char* view_cfs_code(const char* clientNumber)
{
    char* responseData = (char*)malloc(RESPONSE_SIZE * sizeof(char));
    char sql_query[256];
    char forwardNumber[11];
    char *errMsg = 0;
    int returnCode;

    sqlite3_stmt *res;

    sprintf(sql_query, "SELECT isRegistered, isActivated, forwardType FROM forwardinfo");
    sprintf(sql_query + strlen(sql_query), " WHERE clientNumber='%s';", clientNumber);

    //printf("[DEBUG] Prepared String: %s\n", sql_query);
    if (sqlite3_prepare_v2(g_db, sql_query, -1, &res, NULL) != SQLITE_OK) {
        printf("[ERROR] Can't retrieve data: %s\n", sqlite3_errmsg(g_db));
        //sprintf(responseData, "[SERVER] SQL error: %s\n", errMsg);
        sprintf(responseData, "ERROR\n");
        sqlite3_free(errMsg);
        return responseData;
    }

    // Status flag for CFS
    // 0 - None
    // 1 - Registered
    // 2 - Registered and Activated
    int statusFlag=0;

    // For forwarding type
    // 0  CF_NOTSET
    // 1  CF_UNCONDITIONAL
    // 2  CF_BUSY
    // 3  CF_NOREPLY
    int forwType=0;

    while (sqlite3_step(res) == SQLITE_ROW) {
        statusFlag+=atoi(sqlite3_column_text(res, 0));
        statusFlag+=atoi(sqlite3_column_text(res, 1));
        forwType=atoi(sqlite3_column_text(res, 2));
    }
    //printf("Statusflag: %d\n", statusFlag);

    switch(statusFlag)
    {
    case 0: printf("[INFO] User has not registered for CFS\n");
            //sprintf(responseData, "[SERVER] User has not registered for CFS\n");
            break;
    case 1: printf("[INFO] User has not activated CFS\n");
            //sprintf(responseData, "[SERVER] User has not activated CFS\n");
            break;
    case 2: printf("[INFO] User has activated CFS\n");
            //sprintf(responseData, "[SERVER] User has activated CFS\n");
            break;
    }

    if(statusFlag != 2)
    {
        printf("[INFO] Call will be connected to %s\n", clientNumber);
        sprintf(responseData + strlen(responseData), "NF %s\n", clientNumber);
        return responseData;
    }

    sprintf(sql_query, "SELECT forwardNumber FROM forwardinfo");
    sprintf(sql_query + strlen(sql_query), " WHERE clientNumber='%s';", clientNumber);

    //printf("[DEBUG] Prepared String: %s\n", sql_query);
    if (sqlite3_prepare_v2(g_db, sql_query, -1, &res, NULL) != SQLITE_OK) {
        printf("[ERROR] Can't retrieve data: %s\n", sqlite3_errmsg(g_db));
        //sprintf(responseData, "[SERVER] SQL error: %s\n", errMsg);
        sprintf(responseData, "ERROR\n");
        sqlite3_free(errMsg);
        return responseData;
    }

    while (sqlite3_step(res) == SQLITE_ROW) {
    strcpy(forwardNumber,sqlite3_column_text(res, 0));
    }
    switch(forwType)
    {
    case 0: printf("[INFO] Forward Type: Not Set\n");
            printf("[INFO] Call will be connected to %s\n", clientNumber);

            sprintf(responseData + strlen(responseData), "NF %s\n", clientNumber);
            return responseData;
            break;
    case 1: printf("[INFO] Forward Type: Unconditional\n");
            //sprintf(responseData + strlen(responseData), "[SERVER] Forward Type: Unconditional\n");
            sprintf(responseData, "F %s 1\n", forwardNumber);
            break;
    case 2: printf("[INFO] Forward Type: Busy\n");
            //sprintf(responseData + strlen(responseData), "[SERVER] Forward Type: Busy\n");
            sprintf(responseData, "F %s 2\n", forwardNumber);
            break;
    case 3: printf("[INFO] Forward Type: No Reply\n");
            //sprintf(responseData + strlen(responseData), "[SERVER] Forward Type: No Reply\n");
            sprintf(responseData, "F %s 3\n", forwardNumber);
            break;
    }
    printf("[INFO] Call will be forwarded to %s\n", forwardNumber);

    return responseData;
}

// Function to update forwarding number for a client
void update_forwarding_number(const char* clientNumber, const char* forwardingNumber){
    char sql_query[256];
    char *zErrMsg = 0;
    int returnCode;

    sprintf(sql_query, "UPDATE forwardinfo SET ");
    sprintf(sql_query + strlen(sql_query), "forwardNumber='%s'", forwardingNumber);
    sprintf(sql_query + strlen(sql_query), " WHERE clientNumber='%s';", clientNumber);
    //printf("[DEBUG] Prepared String: %s\n", sql_query);
    returnCode = sqlite3_exec(g_db, sql_query, sql_select_callback, 0, &zErrMsg);
    if( returnCode ){
        // Log this section under ERROR
        fprintf(stderr, "[ERROR] Failed to update forwarding number: %s\n", sqlite3_errmsg(g_db));
        sqlite3_free(zErrMsg);
        return;
    }
}

// Function to update call forwarding activation status for a client
void update_activation_status(const char* clientNumber, int isActivated)
{
    char sql_query[256];
    char *zErrMsg = 0;
    int returnCode;

    sprintf(sql_query, "UPDATE forwardinfo SET ");
    sprintf(sql_query + strlen(sql_query), "isActivated=%d", isActivated);
    sprintf(sql_query + strlen(sql_query), " WHERE clientNumber='%s';", clientNumber);
    //printf("[DEBUG] Prepared String: %s\n", sql_query);
    returnCode = sqlite3_exec(g_db, sql_query, sql_select_callback, 0, &zErrMsg);
    if( returnCode ){
        // Log this section under ERROR
        fprintf(stderr, "[ERROR] Failed to activate call forwarding: %s\n", sqlite3_errmsg(g_db));
        sqlite3_free(zErrMsg);
        return;
    }
}

// Function to update forwarding type for a client
void update_forwarding_type(const char* clientNumber, int forwardingType)
{
    char sql_query[256];
    char *zErrMsg = 0;
    int returnCode;

    sprintf(sql_query, "UPDATE forwardinfo SET ");
    sprintf(sql_query + strlen(sql_query), "forwardType=%d", forwardingType);
    sprintf(sql_query + strlen(sql_query), " WHERE clientNumber='%s';", clientNumber);
    //printf("[DEBUG] Prepared String: %s\n", sql_query);
    returnCode = sqlite3_exec(g_db, sql_query, sql_select_callback, 0, &zErrMsg);
    if( returnCode ){
        // Log this section under ERROR
        fprintf(stderr, "[ERROR] Failed to update forwarding type: %s\n", sqlite3_errmsg(g_db));
        sqlite3_free(zErrMsg);
        return;
    }
}

// Function to handle client requests
void handle_client(int client_socket, const char* logFileName) {
    char buffer[1024] = {0};
    char logMsg[128];
    FILE* logger = fopen(logFileName, "a");
    // if (fseek(logger, 0, SEEK_END) != 0) {
    //     perror("[ERROR] Error seeking to end of log file");
    //     sprintf(logMsg, "[SERVER] Error seeking to end of log file. Couldn't process the request.");
    //     send(client_socket, logMsg, strlen(logMsg), 0);
    //     return;
    // }

    char response_message[RESPONSE_SIZE];
    int valread;
    
    valread = read(client_socket, buffer, 1024);
    if (valread <= 0) {
        sprintf(logMsg, "[ERROR] Error reading from client\n");
        fwrite(logMsg, sizeof(char), strlen(logMsg), logger);
        fprintf(stderr, "[ERROR] Error reading from client\n");
        fclose(logger);
        return;
    }

    printf("[INFO] Buffer: %s\n", buffer);
    sprintf(logMsg, "[INFO] Buffer: %s\n", buffer);
    fwrite(logMsg, sizeof(char), strlen(logMsg), logger);

    // It handles these requests
    // ADD_LOGIN    - Add login info for the client number to access CFS system
    // ADD_USER     - Add user info for the client number to the CFS system
    // DEL_USER     - Delete the user details (login + user) from the CFS system
    // VIEW_LOGIN   - View the auth table for all the users in the CFS system
    // VIEW_USER    - View the forwarding table of the CFS system
    // CFS_STATUS   - Check the call forwarding status for a client number
    // CFS_CODE     - For client: Check the call forwarding status for a client number to simulate a phone call
    // UPD_USER     - Update forwarding details for the client number in the CFS system
    // <unknown-req> - else condition: for unknown commmands from the client/admin program
    char *token_params;
    token_params = strtok(buffer, " ");

    // Process the received command
    if (strcmp(token_params, "ADD_LOGIN") == 0) {
        // Example: add_login_details(g_db, "user1", "password1");
        printf("[INFO] Request recieved to add login\n");
        sprintf(logMsg, "[INFO] Request recieved to add login\n");
        fwrite(logMsg, sizeof(char), strlen(logMsg), logger);

        const char* userId = strtok(buffer, " ");
        const char* password = strtok(buffer, " ");
        add_login_details(userId, password);
        sprintf(response_message, "[SERVER] User login details for '%s' has been added to the database\n", userId);
        
    } else if (strcmp(token_params, "ADD_USER") == 0) {
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

        // Example: add_user_data(g_db, 1, "1234567890", 1, 1, "Busy");
        printf("[INFO] Request recieved to add user '%s'\n",clientNumber);
        sprintf(logMsg, "[INFO] Request recieved to add user '%s'\n",clientNumber);
        fwrite(logMsg, sizeof(char), strlen(logMsg), logger);

        add_user_data(clientNumber, forwardingNumber, isRegistered, isActivated, forwardType);
        sprintf(response_message, "[SERVER] User details for '%s' has been added to the database\n", clientNumber);

    }  else if (strcmp(token_params, "DEL_USER") == 0) {
        printf("[INFO] Request recieved to delete user info\n");
        sprintf(logMsg, "[INFO] Request recieved to delete user info\n");
        fwrite(logMsg, sizeof(char), strlen(logMsg), logger);

        char* userId = strtok(NULL, " ");

        if(userId == (int) 0)
        {
            printf("[ERROR] No client number was found in the request.\n");
            sprintf(logMsg, "[ERROR] No client number was found in the request.\n");
            fwrite(logMsg, sizeof(char), strlen(logMsg), logger);
            sprintf(response_message, "[SERVER] No client number was found in the request.\n");
        }
        else{
            printf("[INFO] Request recieved to delete user info of '%s'\n", userId);
            sprintf(logMsg, "[INFO] Request recieved to delete user info of '%s'\n", userId);
            fwrite(logMsg, sizeof(char), strlen(logMsg), logger);

            // Example: delete_user_data(g_db, 1);
            delete_login_details(userId);
            delete_user_data(userId);
            sprintf(response_message, "[SERVER] User details for '%s' has been deleted\n", userId);
        }
        
    } else if (strcmp(token_params, "VIEW_LOGIN") == 0) {
        printf("[INFO] Request recieved to view login data\n");
        sprintf(logMsg, "[INFO] Request recieved to view login data\n");
        fwrite(logMsg, sizeof(char), strlen(logMsg), logger);

        view_auth_table();
    } else if (strcmp(token_params, "VIEW_USER") == 0) {
        printf("[INFO] Request recieved to view user forwarding data\n");
        sprintf(logMsg, "[INFO] Request recieved to view user forwarding data\n");
        fwrite(logMsg, sizeof(char), strlen(logMsg), logger);

        strcpy(response_message, view_forwarding_table());
    } else if (strcmp(token_params, "CFS_STATUS") == 0) {
        char* clientNumber = strtok(NULL, " ");
        printf("[INFO] Request recieved to view user forwarding data of '%s'\n", clientNumber);
        sprintf(logMsg, "[INFO] Request recieved to view user forwarding data of '%s'\n", clientNumber);
        fwrite(logMsg, sizeof(char), strlen(logMsg), logger);

        strcpy(response_message, view_cfs_status(clientNumber));

    } else if (strcmp(token_params, "CFS_CODE") == 0) {
        char* clientNumber = strtok(NULL, " ");
        printf("[INFO] Request recieved to retrieve code for user forwarding data of '%s'\n", clientNumber);
        sprintf(logMsg, "[INFO] Request recieved to retrieve code for user forwarding data of '%s'\n", clientNumber);
        fwrite(logMsg, sizeof(char), strlen(logMsg), logger);

        strcpy(response_message, view_cfs_code(clientNumber));

    } else if (strcmp(token_params, "UPD_USER") == 0) {
        printf("[INFO] Request recieved to update user forwarding data\n");
        sprintf(logMsg, "[INFO] Request recieved to update user forwarding data\n");
        fwrite(logMsg, sizeof(char), strlen(logMsg), logger);

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
            sprintf(response_message, "[SERVER] Forwarding number for '%s' has been updated to '%s'\n", clientNumber, forwardingNumber);
            sprintf(logMsg, "[INFO] Forwarding number for '%s' has been updated to '%s'\n", clientNumber, forwardingNumber);
            fwrite(logMsg, sizeof(char), strlen(logMsg), logger);

        }
        if(isActivated != -1)
        {
            update_activation_status(clientNumber,isActivated);
            printf("[INFO] Updated forwarding activation status\n");
            sprintf(response_message + strlen(response_message), "[SERVER] Activation status for '%s' has been updated to '%s'\n", clientNumber, isActivated ? "ACTIVE" : "NOT ACTIVE");
            sprintf(logMsg, "[INFO] Activation status for '%s' has been updated to '%s'\n", clientNumber, isActivated ? "ACTIVE" : "NOT ACTIVE");
            fwrite(logMsg, sizeof(char), strlen(logMsg), logger);

        }
        if(forwardType != -1)
        {
            update_forwarding_type(clientNumber, forwardType);
            printf("[INFO] Updated forwarding type\n");
            sprintf(logMsg, "[INFO] Updated forwarding type\n");
            fwrite(logMsg, sizeof(char), strlen(logMsg), logger);
            
            sprintf(response_message + strlen(response_message), "[SERVER] Forwarding type for '%s' has been updated to ", clientNumber);
            if (forwardType == 1) {
                sprintf(response_message + strlen(response_message), "'Unconditional'\n");
                sprintf(logMsg, "[INFO] Forward Type: Unconditional\n");
                fwrite(logMsg, sizeof(char), strlen(logMsg), logger);
                
            }
            if (forwardType == 2) {
                sprintf(response_message + strlen(response_message), "'No Reply'\n");
                sprintf(logMsg, "[INFO] Forward Type: No Reply\n");
                fwrite(logMsg, sizeof(char), strlen(logMsg), logger);
            }
            if (forwardType == 3) {
                sprintf(response_message + strlen(response_message), "'Busy'\n");
                sprintf(logMsg, "[INFO] Forward Type: Busy\n");
                fwrite(logMsg, sizeof(char), strlen(logMsg), logger);
            }
        }

    } else {
        fprintf(stderr, "[WARN] Unknown command received from client: %s\n", buffer);
        sprintf(logMsg, "[WARN] Unknown command received from client: %s\n", buffer);
        fwrite(logMsg, sizeof(char), strlen(logMsg), logger);
                
    }

    // while(token_params != NULL)
    // {
    //     token_params = strtok(NULL, " ");
    // }
    
    // Respond back to the client if needed
    // Example: 
    send(client_socket, response_message, strlen(response_message), 0);
    sprintf(logMsg, "[INFO] Sent the following response to the client: \n");
    fwrite(logMsg, sizeof(char), strlen(logMsg), logger);
    fwrite(response_message, sizeof(char), strlen(response_message), logger);
    fclose(logger);
}