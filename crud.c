#include <stdio.h>
#include <string.h>
#include<stdbool.h>

#define FILE_NAME "users.txt"
#define TEMP_FILE "temp.txt"

// checking if the record already exists in the file or not
//i.e if the id is unique or not
struct User
{
    int id;
    char name[50];
    int age;
}user;

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

//creating user
void createUser(){
    FILE *file;
    
    printf("\nEnter the user id: ");
    scanf("%d",&user.id);
    if(idExists(user.id)){
        printf("Error: ID already exists\n");
        return;
    }
    printf("\nEnter the user Name: ");
    scanf("%s",user.name);

    printf("\nEnter the user id: ");
    scanf("%d",&user.age);

    // "a" = appends...It adds to the file or create a new file if the file doesn't exist
    file = fopen(FILE_NAME,"a");

    if(file==NULL){
        printf("Error. Couldn't open file");
        return;
    }
    fprintf(file,"%d %S %d\n", 
        user.id,
        user.name,
        user.age);
    
    fclose(file);
    printf("User created successfully.\n");

}


//Reading the users
void readUsers(){
    FILE *file;
    file= fopen(FILE_NAME,"r");

    // If file is not present obviously no records can be read
    if(file==NULL){
        printf("Error. Couldn't locate file");
        return;
    }
    
    printf("Error. Couldn't locate file");
    // scanf and fscanf returns the number of variables it has read in the form of integer
    while (fscanf(file,"%d %49s %d",
        &user.id,
        user.name
        ,&user.age)==3)
    {
        printf("ID:%d   |   NAME:%s    |    AGE:%d\n",&user.id, user.name, &user.age);
    }
    
    fclose(file);
}

// update the record based on id
void updateUser(){
    FILE *file;
    FILE *temp;

    int id;
    bool found  = false;

    printf("Enter the ID to update the record\n");
    scanf("%d",&id);

    // ID must exist
    if(!idExists(id)){
        printf("Error. ID does not exist.\n");
        return;
    }

    file = fopen(FILE_NAME,"r");

    if(file==NULL){
        printf("Error. Couldn't open file");
        return;
    }

    temp = (TEMP_FILE,"w");

    if(temp==NULL){
        printf("Error. Couldn't create temporary file");
        fclose(file);
        return;
    }

    while (fscanf(file, "%d %49s %d",
                  &user.id,
                  user.name,
                  &user.age) == 3)
    {
        if (user.id == id)
        {
            printf("Enter new Name: ");
            scanf("%49s", user.name);

            printf("Enter new Age: ");
            scanf("%d", &user.age);

            found = true;   
        }

        // Write the record to temporary file. If it was the selected user, the updated    information is written.
        
        fprintf(temp, "%d %s %d\n",
                user.id,
                user.name,
                user.age);
    }

    fclose(file);
    fclose(temp);

    // Replace the old file with the updated temporary file.
    remove(FILE_NAME);
    rename(TEMP_FILE, FILE_NAME);

    if (found)
    {
        printf("User updated successfully.\n");
    }

}

//Delete user
//The whole code is same as updateUser just one change is there

void deleteUser(){
    FILE *file;
    FILE *temp;

    int id;
    bool found  = false;

    printf("Enter the ID to update the record\n");
    scanf("%d",&id);

    // ID must exist
    if(!idExists(id)){
        printf("Error. ID does not exist.\n");
        return;
    }

    file = fopen(FILE_NAME,"r");

    if(file==NULL){
        printf("Error. Couldn't open file");
        return;
    }

    temp = (TEMP_FILE,"w");

    if(temp==NULL){
        printf("Error. Couldn't create temporary file");
        fclose(file);
        return;
    }

    while (fscanf(file, "%d %49s %d",
                  &user.id,
                  user.name,
                  &user.age) == 3)
    {
        // IF found don't write the record into the new temp file
        if (user.id == id)
        {
            found = true; 
            continue;  
        }

        // Write all the other users
        
        fprintf(temp, "%d %s %d\n",
                user.id,
                user.name,
                user.age);
    }

    fclose(file);
    fclose(temp);

    // Replace the old file with the updated temporary file.
    remove(FILE_NAME);
    rename(TEMP_FILE, FILE_NAME);

    if (found)
    {
        printf("User deleted successfully.\n");
    }

}

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