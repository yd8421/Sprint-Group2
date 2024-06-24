#include<admin.h>
#define USERID "admin"
#define PWD "admin@123"

int login(void);
int admin_menu(void);
void add(void);
void delete(void);
void update(void);
void view(void);
void myflush(void);
void fill_command(char[],int);

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

int admin_menu(void){
	
	int try = 4;
	while(try--){

		system("clear");

		int choice;

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

void fill_command(char command[],int choice)
{
	char client_number[11];
	char forwarding_number[11];
	int registered=0;
	int forward_activated=0;
	char forwarding_type;
	int i=0;

	switch(choice)
	{
		case 1: strcpy(command,"VIEW_USER");
		        break;

		case 2: printf("\nEnter client number: ");
			scanf("%s", client_number);
			myflush();
			printf("Enter forwarding number: ");
			scanf(" %s", forwarding_number);
			myflush();
			printf("Register/Unregister user(0/1): ");
			scanf("%d", &registered);
			printf("Activate Service(0/1): ");
			scanf("%d", &forward_activated);
			printf("forward type(U/N/B): ");
			scanf("%c", &forwarding_type);
			myflush();
			strcpy(command, "ADD_USER");
			i += 8;
			command[i++] = ' ';
			
			for(int j=0; j<10; j++){
				command[i++] = clent_number[j];
			}
			command[i++] = ' ';

			for(int j=0; j<n; j++){
				command[i++] = forwarding_number[j];
			}
			command[i++] = ' ';

			command[i++] = '0' + registered;

			break;

		case 3: 
			break;
		case 4: 
			break;
	}
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

