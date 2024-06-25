#include<client.h>

void register_user(char[]);
void myflush(void);
int main_menu(void);
void call(char[]);
int login(void);
int user_menu(void);
void update_client_data(void);
void save(char[]);

char user_no[11];


void myflush(void){
	while(getchar() != '\n');
}

void save(char userNo[]){
	strcpy(user_no, userNo);
}

int login(){
}

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

int user_menu(){
}

void register_user(char command[]){
	
	char client_number[11];
        char forwarding_number[11];
        int registered = 1;
        int forward_activated = 0;
        char forwarding_type;
        int i=0;

        printf("\nEnter client number: ");
        scanf("%s", client_number);
        myflush();

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

void call(char command[]){

	char call_number[11];
	int i = 0;

	system("clear");
	printf("Making call from: %s\n", user_no);
	
	printf("Enter the number to call: ");
	scanf("%s", call_number);

	strcpy(command, "CFS_CODE ");
	
	i = 9;
	for(int j=0; j<strlen(call_number); j++){
		command[i++] = call_number[j];
	}
}

