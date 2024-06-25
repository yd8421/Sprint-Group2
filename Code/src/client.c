#include<client.h>

char user_no[11];

int main(){

	int choice;

	system("clear");
	
	printf("Enter you phone no.: ");
	scanf("%s", user_no);
	myflush();

	save(user_no);
	
	int client_fd;
    	struct sockaddr_in server_addr;
    	char buffer[BUFFER_SIZE];
	char command[BUFFER_SIZE];

    	// Create socket
    	if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        	perror("socket failed");
        	exit(EXIT_FAILURE);
    	}

    	// Prepare server address structure
    	server_addr.sin_family = AF_INET;
    	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    	server_addr.sin_port = htons(PORT);

    	// Connect to server
    	if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        	perror("connect failed");
        	exit(EXIT_FAILURE);
    	}	

    	printf("Connected to server on %s:%d\n", SERVER_IP, PORT);
	
	choice = main_menu();

	if(choice == 1){
		call(command);
		
		char command_copy[40];
		strcpy(command_copy, command);

	        char* call_no = strtok(command_copy, " ");
		call_no = strtok(NULL, " ");
		
		if (send(client_fd, command, strlen(command), 0) < 0) {
       			perror("send failed");
        		exit(EXIT_FAILURE);
    		}
		
		ssize_t bytes_received;
		if ((bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0)) < 0) {
        		perror("recv failed");
        		exit(EXIT_FAILURE);
    		}

    		if (bytes_received == 0) {
        		printf("Server closed connection\n");
    		} else {
        		buffer[bytes_received] = '\0'; // Null-terminate the received data
        		printf("Received response: %s\n", buffer);
    		}
		
		if(strcmp(buffer, "NF\n") == 0){
			system("clear");
			printf("Calling number: %s\n\n", call_no);
		}
		else{
			char* token = strtok(buffer, " ");
			call_no = strtok(NULL, " ");
			int forward_type = atoi(strtok(NULL, " "));
			
			printf("Forwarding call to: %s\n", call_no);
			
			if(forward_type == 1){
				printf("Call connected\n\n");
			}
			else if(forward_type == 2){
				printf("The number you called is BUSY\n\n");
			}
			else{
				printf("The number is NOT RESPONDING\n\n");
			}
			
		}
	}
	else if(choice == 2){
		
		register_user_pass(command);
		
		if (send(client_fd, command, strlen(command), 0) < 0) {
                        perror("send failed");
                        exit(EXIT_FAILURE);
                }

                ssize_t bytes_received;
                if ((bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0)) < 0) {
                        perror("recv failed");
                        exit(EXIT_FAILURE);
                }

                if (bytes_received == 0) {
                        printf("Server closed connection\n");
                } else {
                        buffer[bytes_received] = '\0'; // Null-terminate the received data
                        printf("Received response: %s\n", buffer);
                }
		
		memset(command, '\0', sizeof(command));
		
		register_user(command);

		if (send(client_fd, command, strlen(command), 0) < 0) {
                        perror("send failed");
                        exit(EXIT_FAILURE);
                }

               // ssize_t bytes_received;
                if ((bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0)) < 0) {
                        perror("recv failed");
                        exit(EXIT_FAILURE);
                }

                if (bytes_received == 0) {
                        printf("Server closed connection\n");
                } else {
                        buffer[bytes_received] = '\0'; // Null-terminate the received data
                        printf("Received response: %s\n", buffer);
                }
	}
	else if(choice == 3){
		int ch;
		if(login() == 1){
			ch = user_menu();
		}
	}
	else{
		return 0;
	}
	memset(command, '\0', sizeof(command));
	memset(buffer, '\0', sizeof(buffer));

}
