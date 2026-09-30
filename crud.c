#include <stdio.h>
#include <string.h>

int main(){
    int choice;

    while(1){
        printf("User Management\n");
        printf("1. Create User\n");
        printf("2. Read Users\n");
        printf("3. Update User\n");
        printf("4. Delete User\n");
        printf("5. Exit\n");

        scanf("%d",&choice);
        switch (choice)
        {
        case 1:
            createUser();
            break;
        
        case 2:
            readUsers();
            break;

        case 3:
            updateUser();
            break;

        case 4:
            deleteUser();
            break;

        case 5:
            printf("Exiting....");
            return 0;
        
        default:
            printf("Invalid Choice");
        }
    }
    return 0;
}