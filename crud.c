#include <stdio.h>
#include <string.h>
#include<stdbool.h>

#define FILE_NAME "users.txt"
#define TEMP_FILE "temp.txt"

// checking if the record already exists in the file or not
//i.e if the id is unique or not

bool idExists(int id){
    FILE *file;
    file = fopen(FILE_NAME,"r");
    
    // File doesn't exist i.e. id also doesn't exist
    if(file == NULL){
        return  false;
    }
    while(fscanf(file,"%d %49S %d", 
        &user.id,
        user.name,
        &user.age)==3)
        {
            if(user.id==id){
                fclose(file);
                return true;
            }
        }
        fclose(file);

        return false;
}


struct User
{
    int id;
    char name[50];
    int age;
}user;



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