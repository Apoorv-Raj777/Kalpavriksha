#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 512
#define MAX_NAME 51
#define MAX_INPUT 4096

typedef struct FreeBlock
{
    int index;
    struct FreeBlock *next;
    struct FreeBlock *prev;
} FreeBlock;

typedef struct FileNode
{
    char name[MAX_NAME];

    int isDirectory;
    int contentSize;
    int *blockPointers;
    int blockCount;

    struct FileNode *parent;
    struct FileNode *children;

    struct FileNode *next;
    struct FileNode *prev;
} FileNode;

/* Global variables */

unsigned char (*virtualDisk)[BLOCK_SIZE];

FreeBlock *freeHead = NULL;
FreeBlock *freeTail = NULL;

FileNode *root = NULL;
FileNode *cwd = NULL;

int NUM_BLOCKS;
int usedBlocks = 0;

/* Function prototypes */

void initializeVFS();
FileNode *createNode(char name[], int isDirectory);

void insertChild(FileNode *parent, FileNode *child);
void removeChild(FileNode *parent, FileNode *child);
FileNode *findChild(FileNode *parent, char name[]);

void mkdirCommand(char name[]);
void createCommand(char name[]);
void writeCommand(char name[], char content[]);
void readCommand(char name[]);
void deleteCommand(char name[]);
void rmdirCommand(char name[]);

void lsCommand();
void cdCommand(char name[]);
void pwdCommand();
void dfCommand();

int allocateBlock();
void freeBlock(int index);
void releaseFileBlocks(FileNode *file);

void cleanup(FileNode *node);
void exitVFS();

/* Initialize VFS */

void initializeVFS()
{
    virtualDisk = malloc(NUM_BLOCKS * sizeof(*virtualDisk));

    if (virtualDisk == NULL)
    {
        printf("Virtual disk allocation failed!\n");
        exit(1);
    }

    root = createNode("/", 1);

    if (root == NULL)
    {
        printf("Root directory creation failed!\n");
        free(virtualDisk);
        exit(1);
    }

    cwd = root;

    for (int i = 0; i < NUM_BLOCKS; i++)
    {
        FreeBlock *newBlock = malloc(sizeof(FreeBlock));

        if (newBlock == NULL)
        {
            printf("Free block initialization failed!\n");
            exitVFS();
            exit(1);
        }

        newBlock->index = i;
        newBlock->next = NULL;
        newBlock->prev = freeTail;

        if (freeHead == NULL)
        {
            freeHead = newBlock;
        }
        else
        {
            freeTail->next = newBlock;
        }

        freeTail = newBlock;
    }

    printf("Compact VFS - ready. Type 'exit' to quit.\n");
}

/* Create file or directory node */

FileNode *createNode(char name[], int isDirectory)
{
    FileNode *newNode = malloc(sizeof(FileNode));

    if (newNode == NULL)
    {
        return NULL;
    }

    strcpy(newNode->name, name);

    newNode->isDirectory = isDirectory;
    newNode->contentSize = 0;
    newNode->blockPointers = NULL;
    newNode->blockCount = 0;

    newNode->parent = NULL;
    newNode->children = NULL;

    newNode->next = NULL;
    newNode->prev = NULL;

    return newNode;
}

/* Insert into circular doubly linked list */

void insertChild(FileNode *parent, FileNode *child)
{
    child->parent = parent;

    if (parent->children == NULL)
    {
        parent->children = child;

        child->next = child;
        child->prev = child;
    }
    else
    {
        FileNode *last = parent->children->prev;

        last->next = child;
        child->prev = last;

        child->next = parent->children;
        parent->children->prev = child;
    }
}

/* Remove from circular doubly linked list */

void removeChild(FileNode *parent, FileNode *child)
{
    if (child->next == child)
    {
        parent->children = NULL;
    }
    else
    {
        child->prev->next = child->next;
        child->next->prev = child->prev;

        if (parent->children == child)
        {
            parent->children = child->next;
        }
    }

    child->next = NULL;
    child->prev = NULL;
    child->parent = NULL;
}

/* Search child */

FileNode *findChild(FileNode *parent, char name[])
{
    if (parent->children == NULL)
    {
        return NULL;
    }

    FileNode *temp = parent->children;

    do
    {
        if (strcmp(temp->name, name) == 0)
        {
            return temp;
        }

        temp = temp->next;

    } while (temp != parent->children);

    return NULL;
}

/* Create directory */

void mkdirCommand(char name[])
{
    if (strlen(name) == 0 || strlen(name) >= MAX_NAME)
    {
        printf("Invalid directory name!\n");
        return;
    }

    if (findChild(cwd, name) != NULL)
    {
        printf("Name already exists!\n");
        return;
    }

    FileNode *newNode = createNode(name, 1);

    if (newNode == NULL)
    {
        printf("Directory creation failed!\n");
        return;
    }

    insertChild(cwd, newNode);

    printf("Directory '%s' created successfully.\n", name);
}

/* Create file */

void createCommand(char name[])
{
    if (strlen(name) == 0 || strlen(name) >= MAX_NAME)
    {
        printf("Invalid file name!\n");
        return;
    }

    if (findChild(cwd, name) != NULL)
    {
        printf("Name already exists!\n");
        return;
    }

    FileNode *newNode = createNode(name, 0);

    if (newNode == NULL)
    {
        printf("File creation failed!\n");
        return;
    }

    insertChild(cwd, newNode);

    printf("File '%s' created successfully.\n", name);
}

/* Allocate block from free list */

int allocateBlock()
{
    if (freeHead == NULL)
    {
        return -1;
    }

    FreeBlock *temp = freeHead;
    int index = temp->index;

    freeHead = freeHead->next;

    if (freeHead == NULL)
    {
        freeTail = NULL;
    }
    else
    {
        freeHead->prev = NULL;
    }

    free(temp);

    usedBlocks++;

    return index;
}

/* Return block to free list */

void freeBlock(int index)
{
    FreeBlock *newBlock = malloc(sizeof(FreeBlock));

    if (newBlock == NULL)
    {
        printf("Warning: Unable to free block metadata.\n");
        return;
    }

    newBlock->index = index;
    newBlock->next = NULL;
    newBlock->prev = freeTail;

    if (freeHead == NULL)
    {
        freeHead = newBlock;
    }
    else
    {
        freeTail->next = newBlock;
    }

    freeTail = newBlock;

    usedBlocks--;
}

/* Release all blocks of a file */

void releaseFileBlocks(FileNode *file)
{
    for (int i = 0; i < file->blockCount; i++)
    {
        freeBlock(file->blockPointers[i]);
    }

    free(file->blockPointers);

    file->blockPointers = NULL;
    file->blockCount = 0;
    file->contentSize = 0;
}

/* Write into file */

void writeCommand(char name[], char content[])
{
    FileNode *file = findChild(cwd, name);

    if (file == NULL || file->isDirectory)
    {
        printf("File not found!\n");
        return;
    }

    int size = strlen(content);

    int requiredBlocks = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;

    int available = NUM_BLOCKS - usedBlocks + file->blockCount;

    if (requiredBlocks > available)
    {
        printf("Disk full! Unable to write file.\n");
        return;
    }

    int *newPointers = NULL;

    if (requiredBlocks > 0)
    {
        newPointers = malloc(requiredBlocks * sizeof(int));

        if (newPointers == NULL)
        {
            printf("Memory allocation failed!\n");
            return;
        }
    }

    releaseFileBlocks(file);

    for (int i = 0; i < requiredBlocks; i++)
    {
        newPointers[i] = allocateBlock();

        if (newPointers[i] == -1)
        {
            printf("Block allocation failed!\n");

            for (int j = 0; j < i; j++)
            {
                freeBlock(newPointers[j]);
            }

            free(newPointers);
            return;
        }
    }

    file->blockPointers = newPointers;
    file->blockCount = requiredBlocks;
    file->contentSize = size;

    int position = 0;

    for (int i = 0; i < requiredBlocks; i++)
    {
        int blockIndex = file->blockPointers[i];

        int bytes = size - position;

        if (bytes > BLOCK_SIZE)
        {
            bytes = BLOCK_SIZE;
        }

        memcpy(virtualDisk[blockIndex],
               content + position,
               bytes);

        position += bytes;
    }

    printf("Data written successfully (size=%d bytes).\n", size);
}

/* Read file */

void readCommand(char name[])
{
    FileNode *file = findChild(cwd, name);

    if (file == NULL || file->isDirectory)
    {
        printf("File not found!\n");
        return;
    }

    if (file->contentSize == 0)
    {
        printf("File is empty.\n");
        return;
    }

    int remaining = file->contentSize;

    for (int i = 0; i < file->blockCount; i++)
    {
        int blockIndex = file->blockPointers[i];

        int bytes = remaining;

        if (bytes > BLOCK_SIZE)
        {
            bytes = BLOCK_SIZE;
        }

        fwrite(virtualDisk[blockIndex], 1, bytes, stdout);

        remaining -= bytes;
    }

    printf("\n");
}

/* Delete file */

void deleteCommand(char name[])
{
    FileNode *file = findChild(cwd, name);

    if (file == NULL || file->isDirectory)
    {
        printf("File not found!\n");
        return;
    }

    releaseFileBlocks(file);

    removeChild(cwd, file);

    free(file);

    printf("File deleted successfully.\n");
}

/* Remove empty directory */

void rmdirCommand(char name[])
{
    FileNode *dir = findChild(cwd, name);

    if (dir == NULL || !dir->isDirectory)
    {
        printf("Directory not found!\n");
        return;
    }

    if (dir->children != NULL)
    {
        printf("Directory is not empty!\n");
        return;
    }

    removeChild(cwd, dir);

    free(dir);

    printf("Directory removed successfully.\n");
}

/* List directory contents */

void lsCommand()
{
    if (cwd->children == NULL)
    {
        printf("(empty)\n");
        return;
    }

    FileNode *temp = cwd->children;

    do
    {
        if (temp->isDirectory)
        {
            printf("[DIR]  %s\n", temp->name);
        }
        else
        {
            printf("[FILE] %s\n", temp->name);
        }

        temp = temp->next;

    } while (temp != cwd->children);
}

/* Change directory */

void cdCommand(char name[])
{
    if (strcmp(name, "/") == 0)
    {
        cwd = root;
        printf("Moved to /\n");
        return;
    }

    if (strcmp(name, "..") == 0)
    {
        if (cwd->parent != NULL)
        {
            cwd = cwd->parent;
        }

        printf("Moved to parent directory.\n");
        return;
    }

    FileNode *dir = findChild(cwd, name);

    if (dir == NULL || !dir->isDirectory)
    {
        printf("Directory not found!\n");
        return;
    }

    cwd = dir;

    printf("Moved to directory '%s'.\n", name);
}

/* Display current working directory */

void pwdCommand()
{
    char path[1000] = "";
    char temp[1000] = "";

    FileNode *node = cwd;

    while (node != NULL && node != root)
    {
        strcpy(temp, "/");
        strcat(temp, node->name);
        strcat(temp, path);

        strcpy(path, temp);

        node = node->parent;
    }

    if (strlen(path) == 0)
    {
        printf("/");
    }
    else
    {
        printf("%s", path);
    }
}

/* Display disk usage */

void dfCommand()
{
    int freeBlocks = NUM_BLOCKS - usedBlocks;

    float usage = ((float)usedBlocks / NUM_BLOCKS) * 100;

    printf("\nTotal Blocks: %d\n", NUM_BLOCKS);
    printf("Used Blocks: %d\n", usedBlocks);
    printf("Free Blocks: %d\n", freeBlocks);
    printf("Disk Usage: %.2f%%\n", usage);
}

/* Recursively free directory tree */

void cleanup(FileNode *node)
{
    if (node == NULL)
    {
        return;
    }

    if (node->children != NULL)
    {
        FileNode *first = node->children;
        FileNode *temp = first->next;

        while (temp != first)
        {
            FileNode *nextNode = temp->next;

            cleanup(temp);

            temp = nextNode;
        }

        cleanup(first);
    }

    if (!node->isDirectory)
    {
        free(node->blockPointers);
    }

    free(node);
}

/* Exit and release memory */

void exitVFS()
{
    cleanup(root);

    FreeBlock *temp = freeHead;

    while (temp != NULL)
    {
        FreeBlock *nextBlock = temp->next;

        free(temp);

        temp = nextBlock;
    }

    free(virtualDisk);

    printf("Memory released. Exiting program...\n");
}

/* Main function */

int main()
{
    printf("Enter number of disk blocks (1-5000): ");
    scanf("%d", &NUM_BLOCKS);
    getchar();

    if (NUM_BLOCKS < 1 || NUM_BLOCKS > 5000)
    {
        printf("Invalid number of blocks!\n");
        return 0;
    }

    initializeVFS();

    char input[MAX_INPUT];
    char command[20];
    char name[MAX_NAME];
    char content[MAX_INPUT];

    while (1)
    {
        printf("\n");
        pwdCommand();
        printf(" > ");

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            break;
        }

        input[strcspn(input, "\n")] = '\0';

        command[0] = '\0';
        name[0] = '\0';

        if (sscanf(input, "%19s", command) != 1)
        {
            continue;
        }

        if (strcmp(command, "mkdir") == 0)
        {
            if (sscanf(input, "%*s %50s", name) == 1)
            {
                mkdirCommand(name);
            }
            else
            {
                printf("Usage: mkdir <name>\n");
            }
        }

        else if (strcmp(command, "create") == 0)
        {
            if (sscanf(input, "%*s %50s", name) == 1)
            {
                createCommand(name);
            }
            else
            {
                printf("Usage: create <filename>\n");
            }
        }

        else if (strcmp(command, "write") == 0)
        {
            char *start = strchr(input, '"');
            char *end = strrchr(input, '"');

            if (sscanf(input, "%*s %50s", name) == 1 &&
                start != NULL && end != NULL && end > start)
            {
                int length = end - start - 1;

                if (length >= MAX_INPUT)
                {
                    printf("Content too long!\n");
                    continue;
                }

                strncpy(content, start + 1, length);
                content[length] = '\0';

                writeCommand(name, content);
            }
            else
            {
                printf("Usage: write <filename> \"content\"\n");
            }
        }

        else if (strcmp(command, "read") == 0)
        {
            if (sscanf(input, "%*s %50s", name) == 1)
            {
                readCommand(name);
            }
            else
            {
                printf("Usage: read <filename>\n");
            }
        }

        else if (strcmp(command, "delete") == 0)
        {
            if (sscanf(input, "%*s %50s", name) == 1)
            {
                deleteCommand(name);
            }
            else
            {
                printf("Usage: delete <filename>\n");
            }
        }

        else if (strcmp(command, "rmdir") == 0)
        {
            if (sscanf(input, "%*s %50s", name) == 1)
            {
                rmdirCommand(name);
            }
            else
            {
                printf("Usage: rmdir <directory>\n");
            }
        }

        else if (strcmp(command, "ls") == 0)
        {
            lsCommand();
        }

        else if (strcmp(command, "cd") == 0)
        {
            if (sscanf(input, "%*s %50s", name) == 1)
            {
                cdCommand(name);
            }
            else
            {
                printf("Usage: cd <directory>\n");
            }
        }

        else if (strcmp(command, "pwd") == 0)
        {
            pwdCommand();
        }

        else if (strcmp(command, "df") == 0)
        {
            dfCommand();
        }

        else if (strcmp(command, "exit") == 0)
        {
            exitVFS();
            break;
        }

        else
        {
            printf("Invalid command!\n");
        }
    }

    return 0;
}