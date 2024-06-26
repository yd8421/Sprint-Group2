
#include <admin.h>

int main()
{
    system("clear");
    int try=3, login_status=0;
    while(try--)
    {
        if(login()==1){			//check login authentication
            login_status=1;
            break;
        }
	system("clear");		//to refresh/clear the screen
	printf("You have %d tries left\n\n", try);
            
    }

    if(login_status==0)			//checking if login failed or not
    {
        printf("\nWrong credentials. Exiting the program. \n\n");
        return 0;
    }

    int sock = 0, valread;		//for socket creation, check buffer size
    struct sockaddr_in serv_addr;
    char command[1024] = {0};
    char buffer[BUFFER_LENGTH] = {0};
    
    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, SERVER_ADDRESS, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return -1;
    }
    
    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return -1;
    }
    
    printf("Connected to server on port %d\n", PORT);
    

    while(1){
    	int choice = admin_menu();
    	myflush();
	if(choice == 5){
		send(sock, "EXIT", 4, 0);
		valread = read(sock, buffer, BUFFER_LENGTH);
	 	
		close(sock);
	    	return 0;
    	}
	int ch = create_command_helper(command, choice);

 	if(ch == 0){
		send(sock, "EXIT", 4, 0);
		valread = read(sock, buffer, BUFFER_LENGTH);
		
		close(sock);
	    	return 0;
	}
	
	if(ch == 10){
		
		memset(command, '\0', sizeof(command));
		memset(buffer, '\0', sizeof(buffer));
		
		continue;
	}

    char encr_command[1024];
    strcpy(encr_command, encrypt_string(command));
   	send(sock, encr_command, strlen(encr_command), 0);
    	printf("Sent command: %s\n\n", command);
    	
	valread = read(sock, buffer, BUFFER_LENGTH);

    	if (valread > 0) {
            char decr_buffer[BUFFER_LENGTH];
            //printf("Server response: %s\n\n", buffer);
            strcpy(decr_buffer, decrypt_string(buffer));
            strcpy(buffer, decr_buffer);
        	printf("Server response: %s\n\n", buffer);
    	}

	memset(command, '\0', sizeof(command));
	memset(buffer, '\0', sizeof(buffer));

	printf("\nPress ENTER KEY to continue: ");
    	myflush();
    }

    close(sock);

    return 0;
}	
