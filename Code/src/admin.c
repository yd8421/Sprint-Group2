
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
    admin_menu();
    

    return 0;
}	
