#include<admin.h>
#define USERID "admin"
#define PWD "admin@123"

int login(void);
int admin_menu(void);
int update_menu(void);
void create_command(char[], int);

void myflush(void);
int fill_command(char[],int);

void myflush(void){
	while(getchar() != '\n');
}

int login()
{
    char user[10];
    char pas[20];
    
	
    printf("Enter User ID: ");

    scanf("%s",user);
    printf("Enter Password: ");
    
    scanf("%s",pas);
    
    if(strcmp(user,USERID)==0 && strcmp(pas,PWD)==0)
    {
        return 1;
    }
    else
        return -1;
}

void create_command(char command[], int ch){
	char client_number[11];
	char forwarding_number[11];
	int registered=0;
	int forward_activated=0;
	char forwarding_type;
	int i=0;
        
	printf("\nEnter client number: ");
        scanf("%s", client_number);
        myflush();
        
	if(ch == 0 || ch == 1){
		printf("\nEnter forwarding number: ");
        	scanf(" %s", forwarding_number);
        	myflush();
	}
	else{
		strcpy(forwarding_number, "NA");
	}

        if(ch == 0){
		printf("\nRegister/Unregister user(0/1): ");
        	scanf("%d", &registered);
	}
	else if(ch == 4){
		registered = 0;
	}
	else{
		registered = -1;
	}

        if(ch == 0 || ch == 3){
		printf("\nActivate Service(0/1): ");
        	scanf("%d", &forward_activated);
        	myflush();
	}
	else{
		forward_activated = -1;
	}

        if(ch == 2 || ch == 0){
		printf("\nforward type\nUnconditional - U\nNo reply - N\nBusy - B\n(U/N/B): ");
        	scanf("%c", &forwarding_type);
       		myflush();
	}
	else{
		forwarding_type = 'O';
	}

        if(ch == 0){
		strcpy(command, "ADD_USER");
        	i += 8;
	}
	else{
		strcpy(command, "UPD_USER");
		i += 8;
	}

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


int admin_menu(void){
	
	int try = 4;
	int choice;

	while(try--){

		system("clear");

		printf("******************************************\n");
		printf("          Welcome to Admin Menu            \n");
		printf("******************************************\n\n");
        	printf("\t1. View Record\n");
        	printf("\t2. Add Record\n");
        	printf("\t3. Update Record\n");
        	printf("\t4. Delete Record\n");
		printf("\t5. Exit\n");
		printf("\n******************************************\n");
		
		if(try<3){
			printf("\nWrong input. You have %d tries left. Try Again", try+1);
		}
		printf("\nEnter your choice (1-5): ");

		scanf("%d", &choice);
		myflush();

		if(choice < 1 || choice > 5)continue;
		else break;
	}

	return choice;

}

int update_menu(){

        int try = 3;
        int choice;

        while(try--){

                system("clear");

                printf("******************************************\n");
                printf("               Update Menu                \n");
                printf("******************************************\n\n");
                printf("\t1. Forwarding number\n");
                printf("\t2. Forwarding type\n");
                printf("\t3. Enable/Diasble forwarding\n");
                printf("\t4. Unregister for CFS\n");
                printf("\t5. Back to main menu\n");
                printf("\n******************************************\n");

                if(try<3){
                        printf("\nWrong input. You have %d tries left. Try Again", try+1);
                }
                printf("\nEnter your choice (1-5): ");

                scanf("%d", &choice);
                myflush();

                if(choice < 1 || choice > 5)continue;
                else break;
        }

        return choice;
}

int fill_command(char command[],int choice)
{

	int ch, admin_menu_choice;
	char del_user[11];
	switch(choice)
	{
		case 1: strcpy(command,"VIEW_USER");
		        break;

		case 2: create_command(command, 0);
			break;

		case 3: 
			ch = update_menu();
			if(ch == 5){
				admin_menu_choice = admin_menu();
				if(admin_menu_choice == 5)return 0;
				break;
			}
			create_command(command, ch);
			break;

		case 4: printf("Enter client number to delete: ");
			scanf("%s", del_user);
			myflush();
			strcpy(command, "DEL_USER "); 
			break;
	}
	return 1;
}







void add(void)
{

}

void delete(void)
{

}

void update(void)
{

}

void view(void)
{

}

