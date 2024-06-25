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

	while(1){	
		choice = main_menu();

		if(choice == 1){
			call(command);

			char command_copy[40];
			strcpy(command_copy, command);

	        	char* call_no = strtok(command_copy, " ");
			call_no = strtok(NULL, " ");
				
			send_recv_query(client_fd, command, buffer);
			
			if(strcmp(buffer, "NF\n") == 0){
		//		system("clear");
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
		
			send_recv_query(client_fd, command, buffer);

			if(buffer){
				//already registered, semd to main_menu
			}
			memset(command, '\0', sizeof(command));
			memset(buffer, '\0', sizeof(buffer));

			register_user(command);
	
			send_recv_query(client_fd, command, buffer);

		}
		else if(choice == 3){
			int ch;
			int try = 3;
			int f = 0;
			while(try--){
				
				login(command);
		
				send_recv_query(client_fd, command, buffer);
				
				if(strcmp(buffer, "AUTH_SUCCESS\n") == 0){
					f = 0;
					break;
				}
				else{
					f = 1;
					continue;
				}
			}

			if(f){
				printf("Authentication failed! Redirecting to menu\n");
				
				memset(command, '\0', sizeof(command));
				memset(buffer, '\0', sizeof(buffer));
				
				printf("\nPress ENTER KEY to continue: ");
				myflush();
				
				continue;	
			}


			memset(command, '\0', sizeof(command));
                	memset(buffer, '\0', sizeof(buffer));

			
			ch = user_menu();
		
			if(ch == 5){
				continue;
			}

			if(ch == 6){
			
				send_recv_query(client_fd, "EXIT",  buffer);
				close(client_fd);
				return 0;
			}
		
			create_update_cmd(command, ch);

			send_recv_query(client_fd, command, buffer);

		}
		else{
			send_recv_query(client_fd, "EXIT",  buffer);
			close(client_fd);
			return 0;
		}
		
		memset(command, '\0', sizeof(command));
		memset(buffer, '\0', sizeof(buffer));

		printf("\nPress ENTER KEY to continue: ");
		myflush();

	}
	close(client_fd);
}
