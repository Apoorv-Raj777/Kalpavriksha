#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    int start;
    int size;
    int id;
    int status;       // 0 = Free, 1 = Allocated
} Block;

//  Global variables
unsigned char *memory;
Block *blocks;

int poolSize;
int count = 1;
int capacity = 2;
int nextID = 1;
int strategy = 1;    // 1 = First Fit, 2 = Best Fit, 3 = Worst Fit

// Function prototypes
void initializeMemory();
int findBlock(int size);
int firstFit(int size);
int bestFit(int size);
int worstFit(int size);
void allocateMemory(int size);
void releaseMemory(int id);
void resizeMemory(int id, int newSize);
void displayMap();
void displayStats();
void changeStrategy();
void mergeBlocks();
void insertBlock(int index, Block b);
void removeBlock(int index);
int findID(int id);
void showFragmentation(int size);

/* Initialize memory pool */
void initializeMemory()
{
    memory = (unsigned char *)malloc(poolSize);

    blocks = (Block *)malloc(capacity * sizeof(Block));

    if (memory == NULL || blocks == NULL)
    {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    blocks[0].start = 0;
    blocks[0].size = poolSize;
    blocks[0].id = 0;
    blocks[0].status = 0;

    printf("\nMemory pool initialized successfully!\n");
}

/* First Fit */
int firstFit(int size)
{
    for (int i = 0; i < count; i++)
    {
        if (blocks[i].status == 0 && blocks[i].size >= size)
        {
            return i;
        }
    }

    return -1;
}

/* Best Fit */
int bestFit(int size)
{
    int index = -1;
    int minSize = poolSize + 1;

    for (int i = 0; i < count; i++)
    {
        if (blocks[i].status == 0 && blocks[i].size >= size)
        {
            if (blocks[i].size < minSize)
            {
                minSize = blocks[i].size;
                index = i;
            }
        }
    }

    return index;
}

/* Worst Fit */
int worstFit(int size)
{
    int index = -1;
    int maxSize = -1;

    for (int i = 0; i < count; i++)
    {
        if (blocks[i].status == 0 && blocks[i].size >= size)
        {
            if (blocks[i].size > maxSize)
            {
                maxSize = blocks[i].size;
                index = i;
            }
        }
    }

    return index;
}

/* Select allocation strategy */
int findBlock(int size)
{
    if (strategy == 1)
    {
        return firstFit(size);
    }
    else if (strategy == 2)
    {
        return bestFit(size);
    }
    else
    {
        return worstFit(size);
    }
}

/* Insert a block into metadata array */
void insertBlock(int index, Block b)
{
    if (count == capacity)
    {
        int newCapacity = capacity * 2;

        Block *temp = (Block *)realloc(blocks,
                                      newCapacity * sizeof(Block));

        if (temp == NULL)
        {
            printf("Metadata memory allocation failed!\n");
            return;
        }

        blocks = temp;
        capacity = newCapacity;
    }

    for (int i = count; i > index; i--)
    {
        blocks[i] = blocks[i - 1];
    }

    blocks[index] = b;
    count++;
}

/* Remove a block from metadata array */
void removeBlock(int index)
{
    for (int i = index; i < count - 1; i++)
    {
        blocks[i] = blocks[i + 1];
    }

    count--;
}

/* Allocate memory */
void allocateMemory(int size)
{
    if (size <= 0 || size > poolSize)
    {
        printf("Invalid allocation size!\n");
        return;
    }

    int index = findBlock(size);

    if (index == -1)
    {
        printf("Allocation failed!\n");
        showFragmentation(size);
        return;
    }

    int start = blocks[index].start;

    if (blocks[index].size > size)
    {
        Block newBlock;

        newBlock.start = start + size;
        newBlock.size = blocks[index].size - size;
        newBlock.id = 0;
        newBlock.status = 0;

        insertBlock(index + 1, newBlock);

        if (count == 1 || blocks[index + 1].start != start + size)
        {
            printf("Allocation failed!\n");
            return;
        }
    }

    blocks[index].size = size;
    blocks[index].id = nextID;
    blocks[index].status = 1;

    printf("Memory allocated successfully!\n");
    printf("Allocation ID: %d\n", nextID);
    printf("Starting address: %d\n", start);

    nextID++;
}

/* Find block using allocation ID */
int findID(int id)
{
    for (int i = 0; i < count; i++)
    {
        if (blocks[i].status == 1 && blocks[i].id == id)
        {
            return i;
        }
    }

    return -1;
}

/* Merge adjacent free blocks */
void mergeBlocks()
{
    int i = 0;

    while (i < count - 1)
    {
        if (blocks[i].status == 0 && blocks[i + 1].status == 0)
        {
            blocks[i].size += blocks[i + 1].size;

            removeBlock(i + 1);
        }
        else
        {
            i++;
        }
    }
}

/* Release allocated memory */
void releaseMemory(int id)
{
    int index = findID(id);

    if (index == -1)
    {
        printf("Invalid ID or block already released!\n");
        return;
    }

    blocks[index].status = 0;
    blocks[index].id = 0;

    mergeBlocks();

    printf("Memory released successfully!\n");
}

/* Resize allocated memory */
void resizeMemory(int id, int newSize)
{
    int index = findID(id);

    if (index == -1)
    {
        printf("Invalid allocation ID!\n");
        return;
    }

    if (newSize <= 0 || newSize > poolSize)
    {
        printf("Invalid new size!\n");
        return;
    }

    int oldSize = blocks[index].size;
    int oldStart = blocks[index].start;

    if (newSize == oldSize)
    {
        printf("Size unchanged.\n");
        return;
    }

    /* Shrinking */
    if (newSize < oldSize)
    {
        Block newBlock;

        newBlock.start = oldStart + newSize;
        newBlock.size = oldSize - newSize;
        newBlock.id = 0;
        newBlock.status = 0;

        insertBlock(index + 1, newBlock);

        if (blocks[index + 1].start != oldStart + newSize)
        {
            printf("Resize failed!\n");
            return;
        }

        blocks[index].size = newSize;

        mergeBlocks();

        printf("Memory resized successfully!\n");
        return;
    }

    //  Growing in place
    int extra = newSize - oldSize;

    if (index + 1 < count &&
        blocks[index + 1].status == 0 &&
        blocks[index + 1].size >= extra)
    {
        if (blocks[index + 1].size == extra)
        {
            removeBlock(index + 1);
        }
        else
        {
            blocks[index + 1].start += extra;
            blocks[index + 1].size -= extra;
        }

        blocks[index].size = newSize;

        printf("Memory expanded in place!\n");
        return;
    }

    /* Find another suitable free block */
    int newIndex = findBlock(newSize);

    if (newIndex == -1 || newIndex == index)
    {
        printf("Resize failed! Original allocation remains unchanged.\n");
        return;
    }

    int newStart = blocks[newIndex].start;

    //  Split destination block if necessary 
    if (blocks[newIndex].size > newSize)
    {
        Block freeBlock;

        freeBlock.start = newStart + newSize;
        freeBlock.size = blocks[newIndex].size - newSize;
        freeBlock.id = 0;
        freeBlock.status = 0;

        insertBlock(newIndex + 1, freeBlock);

        if (blocks[newIndex + 1].start != newStart + newSize)
        {
            printf("Resize failed! Original allocation remains unchanged.\n");
            return;
        }
    }

    // Copy existing data to new location
    memcpy(memory + newStart, memory + oldStart, oldSize);

    //  Mark destination as allocated
    blocks[newIndex].size = newSize;
    blocks[newIndex].id = id;
    blocks[newIndex].status = 1;

    // Release old block 
    blocks[index].status = 0;
    blocks[index].id = 0;

    mergeBlocks();

    printf("Memory resized and moved successfully!\n");
    printf("New starting address: %d\n", newStart);
}

//  Display memory map 
void displayMap()
{
    printf("\n========== MEMORY MAP ==========\n");

    printf("%-10s %-10s %-15s %-10s\n",
           "Start", "Size", "Status", "ID");

    for (int i = 0; i < count; i++)
    {
        printf("%-10d %-10d %-15s %-10d\n",
               blocks[i].start,
               blocks[i].size,
               blocks[i].status == 1 ? "Allocated" : "Free",
               blocks[i].id);
    }

    printf("==============================\n");
}

//  Display memory statistics
void displayStats()
{
    int allocated = 0;
    int freeMemory = 0;
    int allocatedBlocks = 0;
    int freeBlocks = 0;
    int largestFree = 0;

    for (int i = 0; i < count; i++)
    {
        if (blocks[i].status == 1)
        {
            allocated += blocks[i].size;
            allocatedBlocks++;
        }
        else
        {
            freeMemory += blocks[i].size;
            freeBlocks++;

            if (blocks[i].size > largestFree)
            {
                largestFree = blocks[i].size;
            }
        }
    }

    printf("\n========== MEMORY STATISTICS ==========\n");

    printf("Total Memory       : %d bytes\n", poolSize);
    printf("Allocated Memory   : %d bytes\n", allocated);
    printf("Free Memory       : %d bytes\n", freeMemory);
    printf("Allocated Blocks   : %d\n", allocatedBlocks);
    printf("Free Blocks       : %d\n", freeBlocks);
    printf("Largest Free Block: %d bytes\n", largestFree);

    printf("====================================\n");
}

// Check external fragmentation
void showFragmentation(int size)
{
    int totalFree = 0;
    int largestFree = 0;

    for (int i = 0; i < count; i++)
    {
        if (blocks[i].status == 0)
        {
            totalFree += blocks[i].size;

            if (blocks[i].size > largestFree)
            {
                largestFree = blocks[i].size;
            }
        }
    }

    if (totalFree >= size && largestFree < size)
    {
        printf("External fragmentation detected!\n");
        printf("Total free memory is sufficient, but not contiguous.\n");
    }
    else
    {
        printf("Insufficient contiguous memory available.\n");
    }
}

//  Change allocation strategy 
void changeStrategy()
{
    int choice;

    printf("\nChoose Allocation Strategy:\n");
    printf("1. First Fit\n");
    printf("2. Best Fit\n");
    printf("3. Worst Fit\n");
    printf("Enter choice: ");

    scanf("%d", &choice);

    if (choice >= 1 && choice <= 3)
    {
        strategy = choice;

        if (strategy == 1)
        {
            printf("Strategy changed to First Fit.\n");
        }
        else if (strategy == 2)
        {
            printf("Strategy changed to Best Fit.\n");
        }
        else
        {
            printf("Strategy changed to Worst Fit.\n");
        }
    }
    else
    {
        printf("Invalid strategy!\n");
    }
}


int main()
{
    printf("Enter total memory pool size: ");
    scanf("%d", &poolSize);

    if (poolSize <= 0 || poolSize > 1000000)
    {
        printf("Invalid pool size! (1 to 1000000)\n");
        return 0;
    }

    initializeMemory();

    int choice;
    int size;
    int id;

    do
    {
        printf("\n===== SMART MEMORY MANAGER =====\n");
        printf("1. Allocate Memory\n");
        printf("2. Release Memory\n");
        printf("3. Resize Memory\n");
        printf("4. Display Memory Map\n");
        printf("5. Display Statistics\n");
        printf("6. Change Allocation Strategy\n");
        printf("7. Exit\n");
        printf("Enter choice: ");

        scanf("%d", &choice);

        if (choice == 1)
        {
            printf("Enter allocation size: ");
            scanf("%d", &size);

            allocateMemory(size);
        }
        else if (choice == 2)
        {
            printf("Enter allocation ID: ");
            scanf("%d", &id);

            releaseMemory(id);
        }
        else if (choice == 3)
        {
            printf("Enter allocation ID: ");
            scanf("%d", &id);

            printf("Enter new size: ");
            scanf("%d", &size);

            resizeMemory(id, size);
        }
        else if (choice == 4)
        {
            displayMap();
        }
        else if (choice == 5)
        {
            displayStats();
        }
        else if (choice == 6)
        {
            changeStrategy();
        }
        else if (choice == 7)
        {
            printf("Exiting Smart Memory Manager...\n");
        }
        else
        {
            printf("Invalid choice!\n");
        }

    } while (choice != 7);

    free(memory);
    free(blocks);

    return 0;
}