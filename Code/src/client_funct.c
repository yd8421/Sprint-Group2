#include<client.h>

void register_user(char[]);
void register_user_pass(char[]);
void myflush(void);
int main_menu(void);
void call(char[]);
void login(char[]);
int user_menu(void);
void create_update_cmd(char[], int);
void save(char[]);
void send_recv_query(int, char[], char[]);
void check(char[]);

char user_no[11];

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

//create a command to check  if the client is already registered
void check(char command[]){
	
	strcpy(command, "CFS_CODE ");
	
	int i = 9;
	for(int j=0; j<strlen(user_no); j++){
		command[i++] = user_no[j];
	}
}

//remove extra enter character
void myflush(void){
	while(getchar() != '\n');
}

//save user_no to global variable
void save(char userNo[]){
	strcpy(user_no, userNo);
}

//send receive query to and from server
void send_recv_query(int client_fd, char command[], char buffer[]){
	char encr_command[1024];
        strcpy(encr_command, encrypt_string(command));
        strcpy(command, encr_command);
        
        if (send(client_fd, command, strlen(command), 0) < 0) { //send query
                        perror("send failed");
                        exit(EXIT_FAILURE);
        }

        ssize_t bytes_received;

        if ((bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0)) < 0) {
                perror("recv failed");
       		exit(EXIT_FAILURE);
        }//recieve from server

        if (bytes_received == 0) {
        	printf("Server closed connection\n");
        } else {
                char decr_buffer[1024];
                
		strcpy(decr_buffer, decrypt_string(buffer));
                strcpy(buffer, decr_buffer);
                buffer[bytes_received] = '\0'; // Null-terminate the received data
                //if(buffer[0] != 'F' || buffer[0] != 'N')
		//	printf("Received response: %s\n", buffer); //print response
        }
}

//create command for login authentication
void login(char command[]){
	
	char password[20];
	int i=0;
	
	system("clear");

	printf("Your userID: %s", user_no);
	printf("\nEnter your password: ");
        scanf("%s", password);
	myflush();
	
	strcpy(command, "AUTH_USER");
	i+=9;
	command[i++] = ' ';

	for(int j=0; j<strlen(user_no); j++){
		command[i++] = user_no[j];
	}
	command[i++] = ' ';

	for(int j=0; j<strlen(password); j++){
		command[i++] = password[j];
	}	
}

//Main client Menu
int main_menu(void){
	
	int choice;
	int try = 4;

	while(try--){

		system("clear");

		printf("******************************************\n");
		printf("          Welcome to Main Menu            \n");
		printf("******************************************\n\n");
        	printf("\t1. Make a phone call\n");
        	printf("\t2. Register for CFS\n");
        	printf("\t3. Login to CFS\n");
        	printf("\t4. Exit\n");
		printf("\n******************************************\n");
		
		if(try<3){
			printf("\nWrong input. You have %d tries left. Try Again", try+1);
		}
		printf("\nEnter your choice (1-4): ");

		scanf("%d", &choice);
		myflush();

		if(choice < 1 || choice > 4){
			continue;
		}
		else break;
	}
	return choice;
}

//After login user menu
int user_menu(){
	int choice;
        int try = 3;

        while(try--){

                system("clear");

                printf("******************************************\n");
                printf("          Welcome to User Menu            \n");
                printf("******************************************\n\n");
                printf("\t1. Change forwarding number\n");
                printf("\t2. Activate/Deactivate forwarding service\n");
                printf("\t3. Change activation type\n");
                printf("\t4. Unregister from the service\n");
		printf("\t5. Logout\n");
		printf("\t6. Exit\n");
                printf("\n******************************************\n");

                if(try<2){
                        printf("\nWrong input. You have %d tries left. Try Again", try+1);
                }
                printf("\nEnter your choice (1-6): ");

                scanf("%d", &choice);
                myflush();

                if(choice < 1 || choice > 6){
                        continue;
                }
                else break;
        }
        return choice;

}

//create command to update user data
void create_update_cmd(char command[], int ch){
	
	char client_number[11];
        char forwarding_number[11];
        int registered=0;
        int forward_activated=0;
        char forwarding_type;
        int i=0;

	strcpy(client_number, user_no);

        if( ch == 1){
                printf("\nEnter forwarding number: ");
                scanf(" %s", forwarding_number);
                myflush();
        }
        else{
                strcpy(forwarding_number, "NA");
        }

       	if(ch == 4){
                registered = 0;
        }
	else if(ch ==10){
		registered = 1;
	}
        else{
                registered = -1;
        }

        if(ch == 2){
                printf("\nActivate Service(0/1): ");
                scanf("%d", &forward_activated);
                myflush();
        }
	else{
                forward_activated = -1;
        }

        if(ch == 3){
                printf("\nforward type\nUnconditional - U\nNo reply - N\nBusy - B\n(U/N/B): ");

                scanf("%c", &forwarding_type);
                myflush();
        }
        else{
                forwarding_type = 'O';
        }

        strcpy(command, "UPD_USER");
      	i += 8;
	command[i++] = ' ';

        for(int j=0; j<strlen(client_number); j++){

                command[i++] = client_number[j];
        }
        command[i++] = ' ';

	for(int j=0; j<strlen(forwarding_number); j++){
                command[i++] = forwarding_number[j];
        }
        command[i++] = ' ';

        if(registered != -1)
                command[i++] = '0' + registered;
        else{
                command[i++] = '-';
                command[i++] = '1';
        }
        command[i++] = ' ';

        if(forward_activated != -1)command[i++] = '0' + forward_activated;
        else{
                command[i++] = '-';
                command[i++] = '1';
        }
        command[i++] = ' ';

        command[i++] = forwarding_type;

}

//create command to register user and password 
void register_user_pass(char command[]){
	
	int f = 0;
	int i = 0;
	char password[20];
	
	while(1){
		char password2[20];
		
		system("clear");

		if(f)printf("Your password didn't match. Try Again:\n");
		printf("\nYour userID: %s", user_no);
		printf("\nEnter Password: ");
		scanf("%s", password);
		myflush();
		printf("Re-Enter password: ");
		scanf("%s", password2);
		myflush();

		if(strcmp(password, password2) == 0){
			break;
		}
		else{
			f = 1;
		}
	}

	strcpy(command, "ADD_LOGIN");
	i+=9;
	command[i++] = ' ';
	
	for(int j=0; j<strlen(user_no); j++){
		command[i++] = user_no[j];
	}
	command[i++] = ' ';

	for(int j=0; j<strlen(password); j++){
		command[i++] = password[j];
	}
	
}

//create command to register user details such forwarding type, forwarding activated, etc
void register_user(char command[]){
	
	char client_number[11];
        char forwarding_number[11];
        int registered = 1;
        int forward_activated = 0;
        char forwarding_type;
        int i=0;

        printf("\nYour userID:  %s", user_no);
	strcpy(client_number, user_no);

      	printf("\nEnter forwarding number: ");
        scanf(" %s", forwarding_number);
        myflush();

       	printf("\nActivate Service(0/1): ");
        scanf("%d", &forward_activated);

       	printf("\nforward type\n");
	printf("Unconditional - U\nNo reply - N\nBusy - B\n(U/N/B): ");
        myflush();
        scanf("%c", &forwarding_type);

        strcpy(command, "ADD_USER");
        i += 8;
        command[i++] = ' ';

        for(int j=0; j<strlen(client_number); j++){

                command[i++] = client_number[j];
        }
        command[i++] = ' ';

        for(int j=0; j<strlen(forwarding_number); j++){
                command[i++] = forwarding_number[j];
        }
        command[i++] = ' ';

        command[i++] = '0' + registered;
	command[i++] = ' ';

        command[i++] = '0' + forward_activated;
	command[i++] = ' ';

        command[i++] = forwarding_type;

}

//create command to authenticate calling number and facilitate forwarding
void call(char command[]){

	char call_number[11];
	int i = 0;

	system("clear");
	printf("Making call from: %s\n", user_no);
	
	printf("Enter the number to call: ");
	scanf("%s", call_number);
	myflush();

	strcpy(command, "CFS_CODE ");
	
	i = 9;
	for(int j=0; j<strlen(call_number); j++){
		command[i++] = call_number[j];
	}
}

