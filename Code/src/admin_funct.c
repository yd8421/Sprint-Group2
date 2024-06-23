#include<admin.h>
#define USERID "admin"
#define PWD "admin@123"
int login(void);
void admin_menu(void);
void add(void);
void delete(void);
void update(void);
void view(void);
void myflush(void);

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

void admin_menu(void){
	
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

		switch(choice){

			case 1: view();
				break;

			case 2: add();
				break;

			case 3: update();
				break;

			case 4: delete();
				break;

			case 5: 
				break;

			default:
				continue;
		}
		break;
	}
	printf("\nExiting the program\n\n");
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

