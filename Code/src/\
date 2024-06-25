
#include <admin.h>

int main()
{
    system("clear");
    int try=3, login_status=0;
    while(try--)
    {
        if(login()==1){
            login_status=1;
            break;
        }
	system("clear");
	printf("You have %d tries left\n\n", try);
            
    }

    if(login_status==0)
    {
        printf("\nWrong credentials. Exiting the program. \n\n");
        return 0;
    }

    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char command[1024] = {0};
    char buffer[1024] = {0};
    
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
    	if(choice == 5){
	 	close(sock);
	    	return 0;
    	}
	int ch = fill_command(command, choice);
 	if(ch == 0){
		close(sock);
	    	return 0;
	}
	
	if(ch == 10){
		continue;
	}

   	send(sock, command, strlen(command), 0);
    	printf("Sent command: %s\n", command);
    
    	valread = read(sock, buffer, 1024);
    	if (valread > 0) {
        	printf("Server response:\n\n%s\n", buffer);
    	}

	memset(command, '\0', sizeof(command));
	printf("Press ENTER KEY to continue: ");
	myflush();
	
    }

    close(sock);

    return 0;
}	
