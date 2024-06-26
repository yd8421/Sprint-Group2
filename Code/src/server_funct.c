#include"server.h"
#include"sqlite3.h"

sqlite3* g_db;

// Function to handle SIGINT signal (Ctrl+C) to close the server
void interrupt_handler(int sig)
{
    signal(sig, SIG_IGN);
    char* currentTime = get_current_time();
    printf("%s [INFO] Interrupt occured: Closing server\n", currentTime);
    printf("%s [INFO] Closing SQLite database connection\n", currentTime);
    free(currentTime);
    close_database();
    exit(0);
}

// Used to get the current time
char* get_current_time() {
    time_t rawTime;
    struct tm *timeInfo;
    char *buffer;

    time(&rawTime);
    timeInfo = localtime(&rawTime);

    buffer = (char*)malloc(20 * sizeof(char));
    if (buffer == NULL) {
        perror("[ERROR] Failed to allocate memory to get current time");
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

// Function to encrypt a string with a ENCRYPT_KEY (offset value)
char* encrypt_string(const char* string)
{
    size_t len = strlen(string);
    char* newString = (char*)malloc((len + 1) * sizeof(char));
    
    strcpy(newString, string);
    
    for(int i=0; i<len; i++){
        newString[i] += 34;
    }
    
    return newString;
}

// Function to decrypt a string with a ENCRYPT_KEY (offset value)
char* decrypt_string(const char* string)
{
    size_t len = strlen(string);
    char* newString = (char*)malloc((len + 1) * sizeof(char));
    
    strcpy(newString, string);
    
    for(int i=0; i<len; i++){
        newString[i] -= 34;
    }
    
    return newString;
}

// Function to handle SQLite errors
void handle_error(sqlite3* g_db) {
    char* currentTime = get_current_time();
    fprintf(stderr, "%s [ERROR] SQLite error: %s\n", currentTime, sqlite3_errmsg(g_db));
    free(currentTime);
    sqlite3_close(g_db);
}

// Function to open SQLite connection
int open_database() {
    int returnCode = sqlite3_open("cfs_data.db", &g_db);
    if (returnCode != SQLITE_OK) {
        char* currentTime = get_current_time();
        fprintf(stderr, "%s [ERROR] Cannot open database: %s\n", currentTime, sqlite3_errmsg(g_db));
        free(currentTime);
        sqlite3_close(g_db);
        return 1;
    }

    char *zErrMsg = 0;
    const char *init_table1 = "CREATE TABLE IF NOT EXISTS forwardinfo (clientNumber TEXT PRIMARY KEY,forwardNumber TEXT,isRegistered INTEGER,isActivated INTEGER,forwardType INTEGER);";
    returnCode = sqlite3_exec(g_db, init_table1, sql_select_callback, 0, &zErrMsg);
    if( returnCode ){
        char* currentTime = get_current_time();
        // Log this section under ERROR
        fprintf(stderr, "%s [FATAL] Failed to create table: %s\n", currentTime, sqlite3_errmsg(g_db));
        free(currentTime);
        return 1;
    }

    const char *init_table2 = "CREATE TABLE IF NOT EXISTS authinfo (clientNumber TEXT PRIMARY KEY,passkey TEXT);";
    returnCode = sqlite3_exec(g_db, init_table2, sql_select_callback, 0, &zErrMsg);
    if( returnCode ){
        char* currentTime = get_current_time();
        // Log this section under ERROR
        fprintf(stderr, "%s [FATAL] Failed to create table: %s\n", currentTime, sqlite3_errmsg(g_db));
        free(currentTime);
        return 1;
    }
    return 0;
}

// Function to close SQLite connection
void close_database() {
    sqlite3_close(g_db);
}

// Function to add login details to SQLite database
char* add_login_details(const char* userId, const char* password)
{
  char sql_query[256];
  char* responseData = (char*)malloc(256 * sizeof(char));
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
        char* currentTime = get_current_time();
        fprintf(stderr, "%s [ERROR] User '%s' is already registered to the CFS\n", currentTime, userId);
        sprintf(responseData, "[ERROR] User '%s' is already registered to the CFS\n", userId);
        free(currentTime);
        //fprintf(stderr, "Failed to log user information, CODE %d: %s\n", returnCode, sqlite3_errmsg(g_db));
        sqlite3_free(zErrMsg);
        return responseData;
    }
    if ( returnCode ) {
        // Log this section under ERROR
        char* currentTime = get_current_time();
        fprintf(stderr, "%s [ERROR] Error occured while adding user '%s'\n", currentTime, userId);
        sprintf(responseData, "[ERROR] Error occured while adding user '%s'\n", userId);
        free(currentTime);
        //fprintf(stderr, "Failed to log user information, CODE %d: %s\n", returnCode, sqlite3_errmsg(g_db));
        sqlite3_free(zErrMsg);
        return responseData;
    }
    else {
        // Message: Client info has been added to the database
        char* currentTime = get_current_time();
        printf("%s [INFO] User login info '%s' has been added to the database\n", currentTime, userId);
        sprintf(responseData, "[SERVER] User login info '%s' has been added to the database\n", userId);
        free(currentTime);
        return responseData;
    }
}

// Function to add user data to SQLite database
char* add_user_data(const char* clientNumber, const char* forwardingNumber, int isRegistered, int isActivated, int forwardingType) {
    char sql_query[256];
    char* responseData = (char*)malloc(256 * sizeof(char));
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
        char* currentTime = get_current_time();
        fprintf(stderr, "%s [ERROR] User '%s' already exist!\n", currentTime, clientNumber);
        sprintf(responseData, "[ERROR] User '%s' already exist!\n", clientNumber);
        free(currentTime);
        //fprintf(stderr, "Failed to log user information, CODE %d: %s\n", returnCode, sqlite3_errmsg(g_db));
        sqlite3_free(zErrMsg);
        return responseData;
    }
    if ( returnCode ) {
        // Log this section under ERROR
        char* currentTime = get_current_time();
        fprintf(stderr, "%s [ERROR] Error occured while adding user '%s'\n", currentTime, clientNumber);
        sprintf(responseData, "[ERROR] Error occured while adding user '%s'\n", clientNumber);
        free(currentTime);
        //fprintf(stderr, "Failed to log user information, CODE %d: %s\n", returnCode, sqlite3_errmsg(g_db));
        sqlite3_free(zErrMsg);
        return responseData;
    }
    else {
        // Message: Client info has been added to the database
        char* currentTime = get_current_time();
        printf("%s [INFO] User info '%s' has been added to the database\n", currentTime, clientNumber);
        sprintf(responseData, "[SERVER] User info '%s' has been added to the database\n", clientNumber);
        free(currentTime);
        return responseData;
    }
}

// Function to delete login details from SQLite database by userId
void delete_login_details(const char* userId) {
    char sql_query[100];
    sprintf(sql_query, "DELETE FROM authinfo WHERE clientNumber = '%s';", userId);
    
    char* errMsg = 0;
    int returnCode = sqlite3_exec(g_db, sql_query, NULL, 0, &errMsg);
    if (returnCode != SQLITE_OK) {
        char* currentTime = get_current_time();
        fprintf(stderr, "%s [ERROR] SQL error: %s\n", currentTime, errMsg);
        free(currentTime);
        sqlite3_free(errMsg);
    } else {
        char* currentTime = get_current_time();
        printf("%s [INFO] Deleted login details successfully.\n", currentTime);
        free(currentTime);
    }
}

// Function to delete user data from SQLite database by client_number
void delete_user_data(const char* client_number) {
    char sql_query[100];
    sprintf(sql_query, "DELETE FROM forwardinfo WHERE clientNumber ='%s';", client_number);
    
    char* errMsg = 0;
    int returnCode = sqlite3_exec(g_db, sql_query, NULL, 0, &errMsg);
    if (returnCode != SQLITE_OK) {
        char* currentTime = get_current_time();
        fprintf(stderr, "%s [ERROR] SQL error: %s\n", currentTime, errMsg);
        free(currentTime);
        sqlite3_free(errMsg);
    } else {
        char* currentTime = get_current_time();
        printf("%s [INFO] Deleted user data successfully.\n", currentTime);
        free(currentTime);
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
        char* currentTime = get_current_time();
        fprintf(stderr, "%s [ERROR] SQL error: %s\n", currentTime, errMsg);
        sprintf(response_data, "[SERVER] SQL error: %s\n", errMsg);
        free(currentTime);
        sqlite3_free(errMsg);
        return response_data;
    }
    
    int tableHeaderStatus=0;
    
    while ((returnCode = sqlite3_step(stmt)) == SQLITE_ROW) {
        if(tableHeaderStatus == 0)
        {
            char* currentTime = get_current_time();
            printf("%s [INFO] Records from the authentication table:\n", currentTime);
            free(currentTime);
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
        char* currentTime = get_current_time();
        fprintf(stderr, "%s [ERROR] SQL error: %s\n", currentTime, errMsg);
        sprintf(response_data, "[SERVER] SQL error: %s\n", errMsg);
        free(currentTime);
        sqlite3_free(errMsg);
        return response_data;
    }
    
    int tableHeaderStatus=0;
    
    while ((returnCode = sqlite3_step(stmt)) == SQLITE_ROW) {
        if(tableHeaderStatus == 0)
        {
            char* currentTime = get_current_time();
            printf("%s [INFO] Records from the forwarding table:\n", currentTime);
            free(currentTime);
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
        char* currentTime = get_current_time();
        printf("%s [INFO] There are no records in the forwarding table\n", currentTime);
        sprintf(response_data, "[SERVER] There are no records in the forwarding table\n");
        free(currentTime);
    }

    sqlite3_finalize(stmt);
    return response_data;
}

char* validate_auth_info(const char* clientNumber, const char* password)
{
    char* responseData = (char*)malloc(RESPONSE_SIZE * sizeof(char));
    char sql_query[256];
    char resp_password[25];
    char *errMsg = 0;
    int returnCode;

    sqlite3_stmt *res;

    sprintf(sql_query, "SELECT passkey FROM authinfo");
    sprintf(sql_query + strlen(sql_query), " WHERE clientNumber='%s';", clientNumber);

    //printf("[DEBUG] Prepared String: %s\n", sql_query);
    if (sqlite3_prepare_v2(g_db, sql_query, -1, &res, NULL) != SQLITE_OK) {
        char* currentTime = get_current_time();
        printf("%s [ERROR] Can't validate login data: %s\n", currentTime, sqlite3_errmsg(g_db));
        sprintf(responseData, "AUTH_FAILURE\n");
        free(currentTime);
        sqlite3_free(errMsg);
        return responseData;
    }

    while (sqlite3_step(res) == SQLITE_ROW) {
        strcpy(resp_password, sqlite3_column_text(res, 0));
    }

    if(strcmp(password, resp_password) == 0){
        char* currentTime = get_current_time();
        printf("%s [INFO] User '%s' had been authenticated\n", currentTime, clientNumber);
        free(currentTime);
        sprintf(responseData, "AUTH_SUCCESS\n");
        return responseData;
    }
    else{
        char* currentTime = get_current_time();
        printf("%s [WARN] User '%s' has entered an invalid password.\n", currentTime, clientNumber);
        free(currentTime);
        sprintf(responseData, "AUTH_INV\n");
        return responseData;
    }
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
        char* currentTime = get_current_time();
        printf("%s [ERROR] Can't retrieve data: %s\n", currentTime, sqlite3_errmsg(g_db));
        free(currentTime);
        sprintf(responseData, "[SERVER] SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return responseData;
    }

    // Status flag for CFS
    // 0 - None
    // 1 - Registered
    // 0 - Not Activated
    // 1 - Activated
    int statusFlag1=0;
    int statusFlag2=0;

    // For forwarding type
    // 0  CF_NOTSET
    // 1  CF_UNCONDITIONAL
    // 2  CF_BUSY
    // 3  CF_NOREPLY
    int forwType=0;

        while (sqlite3_step(res) == SQLITE_ROW) {
        statusFlag1=atoi(sqlite3_column_text(res, 0));
        statusFlag2=atoi(sqlite3_column_text(res, 1));
        forwType=atoi(sqlite3_column_text(res, 2));
    }
    //printf("Statusflag: %d\n", statusFlag);

    char* currentTime = get_current_time();
    switch(statusFlag1)
    {
        case 0: printf("%s [INFO] User has not registered for CFS\n", currentTime);
                sprintf(responseData, "[SERVER] User has not registered for CFS\n");
                break;
        case 1: printf("%s [INFO] User has registered for CFS\n", currentTime);
                sprintf(responseData, "[SERVER] User has registered for CFS\n");
                break;
    }
    switch(statusFlag2)
    {
        case 0: printf("%s [INFO] User has not activated CFS\n", currentTime);
                sprintf(responseData, "[SERVER] User has not activated CFS\n");
                break;
        case 1: printf("%s [INFO] User has activated CFS\n", currentTime);
                sprintf(responseData, "[SERVER] User has activated CFS\n");
                break;
    }
    free(currentTime);

    if(statusFlag1 == 0)
    {
        char* currentTime = get_current_time();
        printf("%s [INFO] Call will be connected to %s\n", currentTime, clientNumber);
        free(currentTime);
        sprintf(responseData + strlen(responseData), "Call will be connected to %s\n", clientNumber);
        return responseData;
    }

    if(statusFlag1 != 1 && statusFlag2 != 1)
    {
        char* currentTime = get_current_time();
        printf("%s [INFO] Call will be connected to %s\n", currentTime, clientNumber);
        free(currentTime);
        sprintf(responseData + strlen(responseData), "Call will be connected to %s\n", clientNumber);
        return responseData;
    }

    sprintf(sql_query, "SELECT forwardNumber FROM forwardinfo");
    sprintf(sql_query + strlen(sql_query), " WHERE clientNumber='%s';", clientNumber);

    //printf("[DEBUG] Prepared String: %s\n", sql_query);
    if (sqlite3_prepare_v2(g_db, sql_query, -1, &res, NULL) != SQLITE_OK) {
        char* currentTime = get_current_time();
        printf("%s [ERROR] Can't retrieve data: %s\n", currentTime, sqlite3_errmsg(g_db));
        free(currentTime);
        sprintf(responseData, "[SERVER] SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return responseData;
    }

    while (sqlite3_step(res) == SQLITE_ROW) {
    strcpy(forwardNumber,sqlite3_column_text(res, 0));
    }

    currentTime = get_current_time();
    switch(forwType)
    {
    case 0: printf("%s [INFO] Forward Type: Not Set\n", currentTime);
            printf("%s [INFO] Call will be connected to %s\n", currentTime, clientNumber);

            sprintf(responseData + strlen(responseData), "[SERVER] Forward Type: Not Set\n");
            sprintf(responseData + strlen(responseData), "[SERVER] Call will be connected to %s\n", clientNumber);
            return responseData;
            break;
    case 1: printf("%s [INFO] Forward Type: Unconditional\n", currentTime);
            sprintf(responseData + strlen(responseData), "[SERVER] Forward Type: Unconditional\n");
            break;
    case 2: printf("%s [INFO] Forward Type: Busy\n", currentTime);
            sprintf(responseData + strlen(responseData), "[SERVER] Forward Type: Busy\n");
            break;
    case 3: printf("%s [INFO] Forward Type: No Reply\n", currentTime);
            sprintf(responseData + strlen(responseData), "[SERVER] Forward Type: No Reply\n");
            break;
    }
    free(currentTime);

    currentTime = get_current_time();
    printf("%s [INFO] Call will be forwarded to %s\n", currentTime, forwardNumber);
    free(currentTime);
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
        char* currentTime = get_current_time();
        printf("%s [ERROR] Can't retrieve data: %s\n", currentTime, sqlite3_errmsg(g_db));
        free(currentTime);
        //sprintf(responseData, "[SERVER] SQL error: %s\n", errMsg);
        sprintf(responseData, "ERROR\n");
        sqlite3_free(errMsg);
        return responseData;
    }

    // Status flag for CFS
    // 0 - None
    // 1 - Registered
    // 0 - Not Activated
    // 1 - Activated
    int statusFlag1=0;
    int statusFlag2=0;

    // For forwarding type
    // 0  CF_NOTSET
    // 1  CF_UNCONDITIONAL
    // 2  CF_BUSY
    // 3  CF_NOREPLY
    int forwType=0;

    while (sqlite3_step(res) == SQLITE_ROW) {
        statusFlag1=atoi(sqlite3_column_text(res, 0));
        statusFlag2=atoi(sqlite3_column_text(res, 1));
        forwType=atoi(sqlite3_column_text(res, 2));
    }
    //printf("Statusflag: %d\n", statusFlag);

    char* currentTime = get_current_time();
    switch(statusFlag1)
    {
        case 0: printf("%s [INFO] User has not registered for CFS\n", currentTime);
                //sprintf(responseData, "[SERVER] User has not registered for CFS\n");
                break;
        case 1: printf("%s [INFO] User has registered for CFS\n", currentTime);
                //sprintf(responseData, "[SERVER] User has not registered for CFS\n");
                break;
    }
    switch(statusFlag2)
    {
        case 0: printf("%s [INFO] User has not activated CFS\n", currentTime);
                //sprintf(responseData, "[SERVER] User has not activated CFS\n");
                break;
        case 1: printf("%s [INFO] User has activated CFS\n", currentTime);
                //sprintf(responseData, "[SERVER] User has activated CFS\n");
                break;
    }

    if(statusFlag1 == 0)
    {
        char* currentTime = get_current_time();
        printf("%s [INFO] Call will be connected to %s\n", currentTime, clientNumber);
        free(currentTime);
        sprintf(responseData + strlen(responseData), "UR\n");
        return responseData;
    }

    if(statusFlag1 != 1 && statusFlag2 != 1)
    {
        char* currentTime = get_current_time();
        printf("%s [INFO] Call will be connected to %s\n", currentTime, clientNumber);
        free(currentTime);
        sprintf(responseData + strlen(responseData), "NF\n");
        return responseData;
    }

    sprintf(sql_query, "SELECT forwardNumber FROM forwardinfo");
    sprintf(sql_query + strlen(sql_query), " WHERE clientNumber='%s';", clientNumber);

    //printf("[DEBUG] Prepared String: %s\n", sql_query);
    if (sqlite3_prepare_v2(g_db, sql_query, -1, &res, NULL) != SQLITE_OK) {
        char* currentTime = get_current_time();
        printf("%s [ERROR] Can't retrieve data: %s\n", currentTime, sqlite3_errmsg(g_db));
        free(currentTime);
        //sprintf(responseData, "[SERVER] SQL error: %s\n", errMsg);
        sprintf(responseData, "ERROR\n");
        sqlite3_free(errMsg);
        return responseData;
    }

    while (sqlite3_step(res) == SQLITE_ROW) {
    strcpy(forwardNumber,sqlite3_column_text(res, 0));
    }

    currentTime = get_current_time();
    switch(forwType)
    {
    case 0: printf("%s [INFO] Forward Type: Not Set\n", currentTime);
            printf("%s [INFO] Call will be connected to %s\n", currentTime, clientNumber);

            sprintf(responseData + strlen(responseData), "NF\n");
            return responseData;
            break;
    case 1: printf("%s [INFO] Forward Type: Unconditional\n", currentTime);
            //sprintf(responseData + strlen(responseData), "[SERVER] Forward Type: Unconditional\n");
            sprintf(responseData, "F %s 1\n", forwardNumber);
            break;
    case 2: printf("%s [INFO] Forward Type: Busy\n", currentTime);
            //sprintf(responseData + strlen(responseData), "[SERVER] Forward Type: Busy\n");
            sprintf(responseData, "F %s 2\n", forwardNumber);
            break;
    case 3: printf("%s [INFO] Forward Type: No Reply\n", currentTime);
            //sprintf(responseData + strlen(responseData), "[SERVER] Forward Type: No Reply\n");
            sprintf(responseData, "F %s 3\n", forwardNumber);
            break;
    }
    free(currentTime);

    currentTime = get_current_time();
    printf("%s [INFO] Call will be forwarded to %s\n", currentTime, forwardNumber);
    free(currentTime);

    return responseData;
}

// Function to update forwarding number for a client
int update_forwarding_number(const char* clientNumber, const char* forwardingNumber){
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
        char* currentTime = get_current_time();
        fprintf(stderr, "%s [ERROR] Failed to update forwarding number: %s\n", currentTime, sqlite3_errmsg(g_db));
        free(currentTime);
        sqlite3_free(zErrMsg);
        return 1;
    }
    return 0;
}

// Function to update registration status for a client
int update_registration_status(const char* clientNumber, int isRegistered)
{
    char sql_query[256];
    char *zErrMsg = 0;
    int returnCode;

    sprintf(sql_query, "UPDATE forwardinfo SET ");
    sprintf(sql_query + strlen(sql_query), "isRegistered=%d", isRegistered);
    sprintf(sql_query + strlen(sql_query), " WHERE clientNumber='%s';", clientNumber);
    //printf("[DEBUG] Prepared String: %s\n", sql_query);
    returnCode = sqlite3_exec(g_db, sql_query, sql_select_callback, 0, &zErrMsg);
    if( returnCode ){
        // Log this section under ERROR
        char* currentTime = get_current_time();
        fprintf(stderr, "%s [ERROR] Failed to update registration status: %s\n", currentTime, sqlite3_errmsg(g_db));
        free(currentTime);
        sqlite3_free(zErrMsg);
        return 1;
    }
    return 0;
}

// Function to update call forwarding activation status for a client
int update_activation_status(const char* clientNumber, int isActivated)
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
        char* currentTime = get_current_time();
        fprintf(stderr, "%s [ERROR] Failed to activate call forwarding: %s\n", currentTime, sqlite3_errmsg(g_db));
        free(currentTime);
        sqlite3_free(zErrMsg);
        return 1;
    }
    return 0;
}

// Function to update forwarding type for a client
int update_forwarding_type(const char* clientNumber, int forwardingType)
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
        char* currentTime = get_current_time();
        fprintf(stderr, "%s [ERROR] Failed to update forwarding type: %s\n", currentTime, sqlite3_errmsg(g_db));
        free(currentTime);
        sqlite3_free(zErrMsg);
        return 1;
    }
    return 0;
}

// Function to handle client requests
int handle_client(int client_socket, const char* logFileName) {
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
        char* currentTime = get_current_time();
        sprintf(logMsg, "%s [ERROR] Error reading from client\n", currentTime);
        fwrite(logMsg, sizeof(char), strlen(logMsg), logger);
        fprintf(stderr, "%s [ERROR] Error reading from client\n", currentTime);
        free(currentTime);
        fclose(logger);
        return 1;
    }
    
    char* currentTime = get_current_time();
    printf("%s [DEBUG] Buffer before decryption: %s\n", currentTime, buffer);
    char* decr_buffer = decrypt_string(buffer);
    strcpy(buffer, decr_buffer);
    printf("%s [DEBUG] Buffer after decryption: %s\n", currentTime, buffer);
    free(currentTime);
    free(decr_buffer);
    if(strcmp(buffer, "EXIT") == 0)
    {
        send(client_socket, "EXIT", strlen("EXIT"), 0);
        fclose(logger);
        return 1;
    }

    currentTime = get_current_time();
    printf("%s [INFO] Buffer: %s\n", currentTime, buffer);
    sprintf(logMsg, "%s [INFO] Buffer: %s\n", currentTime, buffer);
    free(currentTime);
    fwrite(logMsg, sizeof(char), strlen(logMsg), logger);
    

    // It handles these requests
    // ADD_LOGIN    - Add login info for the client number to access CFS system
    // ADD_USER     - Add user info for the client number to the CFS system
    // DEL_USER     - Delete the user details (login + user) from the CFS system
    // VIEW_LOGIN   - View the auth table for all the users in the CFS system
    // VIEW_USER    - View the forwarding table of the CFS system
    // AUTH_USER    - For client: Validate the client based on it's number and the obtained password
    // CFS_STATUS   - Check the call forwarding status for a client number
    // CFS_CODE     - For client: Check the call forwarding status for a client number to simulate a phone call
    // UPD_USER     - Update forwarding details for the client number in the CFS system
    // <unknown-req> - else condition: for unknown commmands from the client/admin program
    char *token_params;
    token_params = strtok(buffer, " ");

    // Process the received command
    // ADD_LOGIN    - Add login info for the client number to access CFS system
    if (strcmp(token_params, "ADD_LOGIN") == 0) {
        // Example: add_login_details(g_db, "user1", "password1");
        char* currentTime = get_current_time();
        printf("%s [INFO] Request recieved to add login\n", currentTime);
        sprintf(logMsg, "%s [INFO] Request recieved to add login\n", currentTime);
        fwrite(logMsg, sizeof(char), strlen(logMsg), logger);
        free(currentTime);

        const char* userId = strtok(NULL, " ");
        const char* password = strtok(NULL, " ");
        
        strcpy(response_message, add_login_details(userId, password));
        
    } 

    // ADD_USER     - Add user info for the client number to the CFS system
    else if (strcmp(token_params, "ADD_USER") == 0) {
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

        strcpy(response_message, add_user_data(clientNumber, forwardingNumber, isRegistered, isActivated, forwardType));

    }
    
    // DEL_USER     - Delete the user details (login + user) from the CFS system
    else if (strcmp(token_params, "DEL_USER") == 0) {
        char* currentTime = get_current_time();
        printf("%s [INFO] Request recieved to delete user info\n", currentTime);
        sprintf(logMsg, "%s [INFO] Request recieved to delete user info\n", currentTime);
        fwrite(logMsg, sizeof(char), strlen(logMsg), logger);
        free(currentTime);

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
        
    } 
    
    // VIEW_LOGIN   - View the auth table for all the users in the CFS system
    else if (strcmp(token_params, "VIEW_LOGIN") == 0) {
        char* currentTime = get_current_time();
        printf("%s [INFO] Request recieved to view login data\n", currentTime);
        sprintf(logMsg, "%s [INFO] Request recieved to view login data\n", currentTime);
        free(currentTime);
        fwrite(logMsg, sizeof(char), strlen(logMsg), logger);

        char *table_data = view_auth_table();
        strcpy(response_message, table_data);
        free(table_data);
    } 
    
    // VIEW_USER    - View the forwarding table of the CFS system
    else if (strcmp(token_params, "VIEW_USER") == 0) {
        char* currentTime = get_current_time();
        printf("%s [INFO] Request recieved to view user forwarding data\n", currentTime);
        sprintf(logMsg, "%s [INFO] Request recieved to view user forwarding data\n", currentTime);
        free(currentTime);
        fwrite(logMsg, sizeof(char), strlen(logMsg), logger);

        char *table_data = view_forwarding_table();
        strcpy(response_message, table_data);
        free(table_data);
    } 
    
    // AUTH_USER    - For client: Validate the client based on it's number and the obtained password
    else if (strcmp(token_params, "AUTH_USER") == 0) {
        char* clientNumber = strtok(NULL, " ");
        char* password = strtok(NULL, " ");
        char* currentTime = get_current_time();
        printf("%s [INFO] Request recieved to authenticate user\n", currentTime);
        sprintf(logMsg, "%s [INFO] Request recieved to authenticate user\n", currentTime);
        free(currentTime);
        fwrite(logMsg, sizeof(char), strlen(logMsg), logger);

        strcpy(response_message, validate_auth_info(clientNumber, password));
    } 
    
    // CFS_STATUS   - Check the call forwarding status for a client number
    else if (strcmp(token_params, "CFS_STATUS") == 0) {
        char* clientNumber = strtok(NULL, " ");
        char* currentTime = get_current_time();
        printf("%s [INFO] Request recieved to view user forwarding data of '%s'\n", currentTime, clientNumber);
        sprintf(logMsg, "%s [INFO] Request recieved to view user forwarding data of '%s'\n", currentTime, clientNumber);
        free(currentTime);
        fwrite(logMsg, sizeof(char), strlen(logMsg), logger);

        strcpy(response_message, view_cfs_status(clientNumber));

    } 
    
    // CFS_CODE     - For client: Check the call forwarding status for a client number to simulate a phone call
    else if (strcmp(token_params, "CFS_CODE") == 0) {
        char* clientNumber = strtok(NULL, " ");
        char* currentTime = get_current_time();
        printf("%s [INFO] Call: Request recieved to retrieve code for user forwarding data of '%s'\n", currentTime, clientNumber);
        sprintf(logMsg, "%s [INFO] Call: Request recieved to retrieve code for user forwarding data of '%s'\n", currentTime, clientNumber);
        free(currentTime);
        fwrite(logMsg, sizeof(char), strlen(logMsg), logger);

        strcpy(response_message, view_cfs_code(clientNumber));

    } 
    
    // UPD_USER     - Update forwarding details for the client number in the CFS system
    else if (strcmp(token_params, "UPD_USER") == 0) {
        memset(response_message, '\0', sizeof(response_message));
        char* currentTime = get_current_time();
        printf("%s [INFO] Request recieved to update user forwarding data\n", currentTime);
        sprintf(logMsg, "%s [INFO] Request recieved to update user forwarding data\n", currentTime);
        free(currentTime);
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
            if(update_forwarding_number(clientNumber,forwardingNumber)){
                printf("[ERROR] Failed to update forwarding number for '%s'\n", clientNumber);
                sprintf(response_message, "[SERVER] Failed to update forwarding number for '%s'\n", clientNumber);
                sprintf(logMsg, "[ERROR] Failed to update forwarding number for '%s'\n", clientNumber);
                fwrite(logMsg, sizeof(char), strlen(logMsg), logger);
            }
            else{
                printf("[INFO] Updated forwarding number\n");
                sprintf(response_message, "[SERVER] Forwarding number for '%s' has been updated to '%s'\n", clientNumber, forwardingNumber);
                sprintf(logMsg, "[INFO] Forwarding number for '%s' has been updated to '%s'\n", clientNumber, forwardingNumber);
                fwrite(logMsg, sizeof(char), strlen(logMsg), logger);
            }

        }
        if(isRegistered != -1)
        {
            if(update_registration_status(clientNumber,isRegistered)){
                printf("[ERROR] Failed to update registration status for user '%s'\n", clientNumber);
                sprintf(response_message + strlen(response_message), "[SERVER] Failed to update registration status for user '%s'\n", clientNumber);
                sprintf(logMsg, "[ERROR] Failed to update registration status for user '%s'\n", clientNumber);
                fwrite(logMsg, sizeof(char), strlen(logMsg), logger);
            }
            else {
                printf("[INFO] Registration status for '%s' has been updated to '%s'\n", clientNumber, isRegistered ? "REGISTERED" : "UNREGISTERED");
                sprintf(response_message + strlen(response_message), "[SERVER] Registration status for '%s' has been updated to '%s'\n", clientNumber, isRegistered ? "REGISTERED" : "UNREGISTERED");
                sprintf(logMsg, "[INFO] Registration status for '%s' has been updated to '%s'\n", clientNumber, isRegistered ? "REGISTERED" : "UNREGISTERED");
                fwrite(logMsg, sizeof(char), strlen(logMsg), logger);
            }

        }
        if(isActivated != -1)
        {
            if(update_activation_status(clientNumber,isActivated)){
                printf("[ERROR] Failed to update forwarding activation status for '%s'\n", clientNumber);
                sprintf(response_message + strlen(response_message), "[SERVER] Failed to update forwarding activation status for '%s'\n", clientNumber);
                sprintf(logMsg, "[ERROR] Failed to update forwarding activation status for '%s'\n", clientNumber);
                fwrite(logMsg, sizeof(char), strlen(logMsg), logger);
            }
            else {
                printf("[INFO] Updated forwarding activation status\n");
                sprintf(response_message + strlen(response_message), "[SERVER] Activation status for '%s' has been updated to '%s'\n", clientNumber, isActivated ? "ACTIVE" : "NOT ACTIVE");
                sprintf(logMsg, "[INFO] Activation status for '%s' has been updated to '%s'\n", clientNumber, isActivated ? "ACTIVE" : "NOT ACTIVE");
                fwrite(logMsg, sizeof(char), strlen(logMsg), logger);
            }

        }
        if(forwardType != -1)
        {
            if(update_forwarding_type(clientNumber, forwardType)){
                printf("[ERROR] Failed to update forwarding type for '%s'\n", clientNumber);
                sprintf(response_message + strlen(response_message), "[SERVER] Failed to update forwarding type for '%s'\n", clientNumber);
                sprintf(logMsg, "[ERROR] Failed to update forwarding type for '%s'\n", clientNumber);
                fwrite(logMsg, sizeof(char), strlen(logMsg), logger);
            }
            else {        
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
        }

    } 
    
    // <unknown-req> - else condition: for unknown commmands from the client/admin program
    else {
        char* currentTime = get_current_time();
        fprintf(stderr, "%s [WARN] Unknown command received from client: %s\n", currentTime, buffer);
        sprintf(logMsg, "%s [WARN] Unknown command received from client: %s\n", currentTime, buffer);
        free(currentTime);
        fwrite(logMsg, sizeof(char), strlen(logMsg), logger);
    }

    // while(token_params != NULL)
    // {
    //     token_params = strtok(NULL, " ");
    // }
    
    // Respond back to the client if needed
    // Example: 
    char* encr_response_message = encrypt_string(response_message);
    send(client_socket, encr_response_message, strlen(response_message), 0);

    currentTime = get_current_time();
    sprintf(logMsg, "%s [DEBUG] Sent the following response to the client: \n", currentTime);
    fwrite(logMsg, sizeof(char), strlen(logMsg), logger);
    fwrite(response_message, sizeof(char), strlen(response_message), logger);

    sprintf(logMsg, "%s [DEBUG] Sent the following response to the client: \n", currentTime);
    fwrite(logMsg, sizeof(char), strlen(logMsg), logger);
    fwrite(encr_response_message, sizeof(char), strlen(encr_response_message), logger);

    free(currentTime);

    sprintf(logMsg, "\n");
    fwrite(logMsg, sizeof(char), strlen(logMsg), logger);

    free(encr_response_message);
    fclose(logger);
    return 0;
}