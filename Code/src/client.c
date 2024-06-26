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
			
			int flag =0;

			check(command);
			
			send_recv_query(client_fd, command, buffer);
			
			if(buffer[0] == 'U')flag =1;

			memset(command, '\0', sizeof(command));
			memset(buffer, '\0', sizeof(buffer));
			
			call(command);

			char command_copy[40];
			strcpy(command_copy, command);

	        	char* call_no = strtok(command_copy, " ");
			call_no = strtok(NULL, " ");
				
			send_recv_query(client_fd, command, buffer);
			
			if(flag == 1 || strcmp(buffer, "NF\n") == 0 || strcmp(buffer, "UR\n") == 0){
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
				else if(forward_type == 3){
					printf("The number you called is BUSY\n\n");
				}
				else{
					sleep(3);
					printf("The number is NOT RESPONDING\n\n");
				}
			
			}
		}
		else if(choice == 2){
			
			check(command);
			
			send_recv_query(client_fd, command, buffer);
			
			if(buffer[0] == 'F' || buffer[0] == 'N' ){
				
				printf("\nWe are registrating you with your previous settings\n");
				
				memset(command, '\0', sizeof(command));	
				memset(buffer, '\0', sizeof(command));	
				
				char comm[11] = "NA 1 -1 O";

				strcpy(command, "UPD_USER ");
				int i = 9;

				for(int j=0; j<strlen(user_no); j++){
					command[i++] = user_no[j];
				}
				command[i++] = ' ';

				for(int j=0; j<strlen(comm); j++){
					command[i++] = comm[j];
				}

				send_recv_query(client_fd, command, buffer);
				
				memset(command, '\0', sizeof(command));	
				memset(buffer, '\0', sizeof(command));	
				
				printf("\nPress ENTER KEY to continue: ");
				myflush();
				
				continue;	
			}

			memset(command, '\0', sizeof(command));	
			memset(buffer, '\0', sizeof(command));	

			register_user_pass(command);
		
			send_recv_query(client_fd, command, buffer);

			char* token = strtok(buffer, " ");

			memset(command, '\0', sizeof(command));

			if(strcmp(token, "[ERROR]") == 0){
			 	
				printf("\nUser already Registered\nLogin from the Main Menu\n");
				
				printf("\nPress ENTER KEY to continue: ");
				memset(buffer, '\0', sizeof(buffer));
			
				myflush();
				continue;
			}
			
			memset(buffer, '\0', sizeof(buffer));

			register_user(command);
	
			send_recv_query(client_fd, command, buffer);
			myflush();

		}
		else if(choice == 3){
			int ch;
			int try = 3;
			int f = 0;
			while(try--){
				
				if(try<2)printf("Wrong password. Try again\n\n");

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
			
				send_recv_query(client_fd, encrypt_string("EXIT"),  buffer);
				close(client_fd);
				return 0;
			}
		
			create_update_cmd(command, ch);
			printf("buffer %s", buffer);
			send_recv_query(client_fd, command, buffer);

		}
		else{
			send_recv_query(client_fd, encrypt_string("EXIT"),  buffer);
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
