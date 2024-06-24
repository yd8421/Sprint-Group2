#include<client.h>

void register_user(void);
static void myflush(void);
void main_menu(void);
void call(void);
void register_user(void);
void login(void);
void user_menu(void);
void update_client_data(void);

static void myflush(void){
	while(getchar() != '\n');
}

void main_menu(void){
	
	int try = 4;
	while(try--){

		system("clear");

		int choice;

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

		switch(choice){

			case 1: call();
				break;

			case 2: register_user();
				break;

			case 3: login();
				break;

			case 4: 
				break;

			default:
				continue;
		}
		break;
	}
	printf("\nExiting the program\n\n");
}

void login(void){}
void register_user(void){}
void call(void){}

