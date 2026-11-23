#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Players_data.h"

#define TEAM_COUNT 10
#define MAX_NAME 51

/* Separate player structure */

typedef struct
{
    int PlayerId;
    char Name[MAX_NAME];
    char TeamName[MAX_NAME];
    int Role;

    int TotalRuns;
    float BattingAverage;
    float StrikeRate;
    int Wickets;
    float EconomyRate;
    float PerformanceIndex;

} ODIPlayer;


/* Player linked list node */

typedef struct PlayerNode
{
    ODIPlayer player;

    struct PlayerNode *nextAll;
    struct PlayerNode *nextRole;

} PlayerNode;


/* Team structure */

typedef struct
{
    int TeamId;
    char Name[MAX_NAME];

    int TotalPlayers;
    float AverageBattingStrikeRate;

    PlayerNode *allPlayers;

    PlayerNode *roleHead[3];

} Team;


/* Heap node for merging */

typedef struct
{
    PlayerNode *node;
    int teamIndex;

} HeapNode;


/* Global variables */

Team teamList[TEAM_COUNT];

int totalPlayers = 0;


/* Function prototypes */

void initializeTeams();
void initializePlayers();

float calculatePerformance(ODIPlayer p);

int getRole(char role[]);
char *getRoleName(int role);

int searchTeam(int id);
int playerExists(int id);

void insertPlayer(PlayerNode *newNode, int teamIndex);

void addPlayer();
void displayTeamPlayers();
void displayTeamsByStrikeRate();

void displayTopKPlayers();
void displayAllPlayersByRole();

void calculateTeamAverage(int index);

void printPlayer(ODIPlayer p);
void printPlayerHeader();

void swapTeams(Team *a, Team *b);

void heapify(HeapNode heap[], int n, int i);
void insertHeap(HeapNode heap[], int *size, HeapNode value);
HeapNode removeHeap(HeapNode heap[], int *size);


/* Calculate performance index */

float calculatePerformance(ODIPlayer p)
{
    if (p.Role == 1)
    {
        return (p.BattingAverage * p.StrikeRate) / 100;
    }

    else if (p.Role == 2)
    {
        return (p.Wickets * 2) + (100 - p.EconomyRate);
    }

    else
    {
        return ((p.BattingAverage * p.StrikeRate) / 100)
               + (p.Wickets * 2);
    }
}


/* Convert role string into integer */

int getRole(char role[])
{
    if (strcmp(role, "Batsman") == 0)
    {
        return 1;
    }

    else if (strcmp(role, "Bowler") == 0)
    {
        return 2;
    }

    else if (strcmp(role, "All-rounder") == 0)
    {
        return 3;
    }

    return 0;
}


/* Convert integer into role name */

char *getRoleName(int role)
{
    if (role == 1)
    {
        return "Batsman";
    }

    else if (role == 2)
    {
        return "Bowler";
    }

    else
    {
        return "All-rounder";
    }
}


/* Initialize teams */

void initializeTeams()
{
    for (int i = 0; i < TEAM_COUNT; i++)
    {
        teamList[i].TeamId = i + 1;

        strcpy(teamList[i].Name, teams[i]);

        teamList[i].TotalPlayers = 0;

        teamList[i].AverageBattingStrikeRate = 0;

        teamList[i].allPlayers = NULL;

        for (int j = 0; j < 3; j++)
        {
            teamList[i].roleHead[j] = NULL;
        }
    }
}


/* Search team using binary search */

int searchTeam(int id)
{
    int low = 0;
    int high = TEAM_COUNT - 1;

    while (low <= high)
    {
        int mid = low + (high - low) / 2;

        if (teamList[mid].TeamId == id)
        {
            return mid;
        }

        else if (teamList[mid].TeamId < id)
        {
            low = mid + 1;
        }

        else
        {
            high = mid - 1;
        }
    }

    return -1;
}


/* Check duplicate player ID */

int playerExists(int id)
{
    for (int i = 0; i < TEAM_COUNT; i++)
    {
        PlayerNode *temp = teamList[i].allPlayers;

        while (temp != NULL)
        {
            if (temp->player.PlayerId == id)
            {
                return 1;
            }

            temp = temp->nextAll;
        }
    }

    return 0;
}


/* Insert player into linked lists */

void insertPlayer(PlayerNode *newNode, int teamIndex)
{
    Team *team = &teamList[teamIndex];

    /* Insert into general player list */

    newNode->nextAll = team->allPlayers;

    team->allPlayers = newNode;


    /* Insert into role-wise sorted list */

    int roleIndex = newNode->player.Role - 1;

    PlayerNode **head = &team->roleHead[roleIndex];

    if (*head == NULL ||
        (*head)->player.PerformanceIndex <
        newNode->player.PerformanceIndex)
    {
        newNode->nextRole = *head;

        *head = newNode;
    }

    else
    {
        PlayerNode *temp = *head;

        while (temp->nextRole != NULL &&
               temp->nextRole->player.PerformanceIndex >=
               newNode->player.PerformanceIndex)
        {
            temp = temp->nextRole;
        }

        newNode->nextRole = temp->nextRole;

        temp->nextRole = newNode;
    }

    team->TotalPlayers++;

    totalPlayers++;
}


/* Initialize players from header file */

void initializePlayers()
{
    for (int i = 0; i < playerCount; i++)
    {
        ODIPlayer p;

        p.PlayerId = players[i].id;

        strcpy(p.Name, players[i].name);

        strcpy(p.TeamName, players[i].team);

        p.Role = getRole((char *)players[i].role);

        p.TotalRuns = players[i].totalRuns;

        p.BattingAverage = players[i].battingAverage;

        p.StrikeRate = players[i].strikeRate;

        p.Wickets = players[i].wickets;

        p.EconomyRate = players[i].economyRate;

        p.PerformanceIndex = calculatePerformance(p);

        int teamIndex = -1;

        for (int j = 0; j < TEAM_COUNT; j++)
        {
            if (strcmp(teamList[j].Name, p.TeamName) == 0)
            {
                teamIndex = j;
                break;
            }
        }

        if (teamIndex != -1)
        {
            PlayerNode *newNode = malloc(sizeof(PlayerNode));

            if (newNode == NULL)
            {
                printf("Memory allocation failed!\n");
                exit(1);
            }

            newNode->player = p;

            newNode->nextAll = NULL;
            newNode->nextRole = NULL;

            insertPlayer(newNode, teamIndex);
        }
    }

    printf("Players and teams initialized successfully!\n");
}


/* Calculate average batting strike rate */

void calculateTeamAverage(int index)
{
    PlayerNode *temp = teamList[index].allPlayers;

    float sum = 0;
    int count = 0;

    while (temp != NULL)
    {
        if (temp->player.Role == 1 ||
            temp->player.Role == 3)
        {
            sum += temp->player.StrikeRate;

            count++;
        }

        temp = temp->nextAll;
    }

    if (count > 0)
    {
        teamList[index].AverageBattingStrikeRate = sum / count;
    }

    else
    {
        teamList[index].AverageBattingStrikeRate = 0;
    }
}


/* Add new player */

void addPlayer()
{
    int teamId;

    printf("Enter Team ID: ");
    scanf("%d", &teamId);

    int index = searchTeam(teamId);

    if (index == -1)
    {
        printf("Invalid Team ID!\n");
        return;
    }

    ODIPlayer p;

    printf("Player ID: ");
    scanf("%d", &p.PlayerId);

    if (p.PlayerId < 1 || p.PlayerId > 1000 ||
        playerExists(p.PlayerId))
    {
        printf("Invalid or duplicate Player ID!\n");
        return;
    }

    printf("Name: ");
    scanf(" %50[^\n]", p.Name);

    printf("Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    scanf("%d", &p.Role);

    if (p.Role < 1 || p.Role > 3)
    {
        printf("Invalid role!\n");
        return;
    }

    printf("Total Runs: ");
    scanf("%d", &p.TotalRuns);

    printf("Batting Average: ");
    scanf("%f", &p.BattingAverage);

    printf("Strike Rate: ");
    scanf("%f", &p.StrikeRate);

    printf("Wickets: ");
    scanf("%d", &p.Wickets);

    printf("Economy Rate: ");
    scanf("%f", &p.EconomyRate);

    strcpy(p.TeamName, teamList[index].Name);

    p.PerformanceIndex = calculatePerformance(p);

    PlayerNode *newNode = malloc(sizeof(PlayerNode));

    if (newNode == NULL)
    {
        printf("Memory allocation failed!\n");
        return;
    }

    newNode->player = p;

    newNode->nextAll = NULL;
    newNode->nextRole = NULL;

    insertPlayer(newNode, index);

    printf("\nPlayer added successfully to Team %s!\n",
           teamList[index].Name);
}


/* Print table heading */

void printPlayerHeader()
{
    printf("\n");
    printf("============================================================================================\n");

    printf("%-6s %-25s %-13s %-8s %-8s %-8s %-8s %-8s %-10s\n",
           "ID", "Name", "Role", "Runs", "Avg", "SR",
           "Wkts", "ER", "Perf.Index");

    printf("============================================================================================\n");
}


/* Print player details */

void printPlayer(ODIPlayer p)
{
    printf("%-6d %-25s %-13s %-8d %-8.2f %-8.2f %-8d %-8.2f %-10.2f\n",
           p.PlayerId,
           p.Name,
           getRoleName(p.Role),
           p.TotalRuns,
           p.BattingAverage,
           p.StrikeRate,
           p.Wickets,
           p.EconomyRate,
           p.PerformanceIndex);
}


/* Display players of a team */

void displayTeamPlayers()
{
    int id;

    printf("Enter Team ID: ");
    scanf("%d", &id);

    int index = searchTeam(id);

    if (index == -1)
    {
        printf("Invalid Team ID!\n");
        return;
    }

    Team *team = &teamList[index];

    printf("\nPlayers of Team %s:\n", team->Name);

    printPlayerHeader();

    PlayerNode *temp = team->allPlayers;

    while (temp != NULL)
    {
        printPlayer(temp->player);

        temp = temp->nextAll;
    }

    calculateTeamAverage(index);

    printf("============================================================================================\n");

    printf("Total Players: %d\n", team->TotalPlayers);

    printf("Average Batting Strike Rate: %.2f\n",
           team->AverageBattingStrikeRate);
}


/* Swap teams */

void swapTeams(Team *a, Team *b)
{
    Team temp = *a;

    *a = *b;

    *b = temp;
}


/* Display teams sorted by average strike rate */

void displayTeamsByStrikeRate()
{
    Team sortedTeams[TEAM_COUNT];

    for (int i = 0; i < TEAM_COUNT; i++)
    {
        calculateTeamAverage(i);

        sortedTeams[i] = teamList[i];
    }

    /* Bubble sort in descending order */

    for (int i = 0; i < TEAM_COUNT - 1; i++)
    {
        for (int j = 0; j < TEAM_COUNT - i - 1; j++)
        {
            if (sortedTeams[j].AverageBattingStrikeRate <
                sortedTeams[j + 1].AverageBattingStrikeRate)
            {
                swapTeams(&sortedTeams[j], &sortedTeams[j + 1]);
            }
        }
    }

    printf("\nTeams Sorted by Average Batting Strike Rate\n");

    printf("============================================================\n");

    printf("%-6s %-20s %-15s %-10s\n",
           "ID", "Team Name", "Avg Bat SR", "Players");

    printf("============================================================\n");

    for (int i = 0; i < TEAM_COUNT; i++)
    {
        printf("%-6d %-20s %-15.2f %-10d\n",
               sortedTeams[i].TeamId,
               sortedTeams[i].Name,
               sortedTeams[i].AverageBattingStrikeRate,
               sortedTeams[i].TotalPlayers);
    }

    printf("============================================================\n");
}


/* Display top K players of a team */

void displayTopKPlayers()
{
    int id, role, k;

    printf("Enter Team ID: ");
    scanf("%d", &id);

    int index = searchTeam(id);

    if (index == -1)
    {
        printf("Invalid Team ID!\n");
        return;
    }

    printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    scanf("%d", &role);

    if (role < 1 || role > 3)
    {
        printf("Invalid role!\n");
        return;
    }

    printf("Enter number of players K: ");
    scanf("%d", &k);

    if (k <= 0)
    {
        printf("Invalid K!\n");
        return;
    }

    PlayerNode *temp = teamList[index].roleHead[role - 1];

    printf("\nTop %d %s players of Team %s:\n",
           k, getRoleName(role), teamList[index].Name);

    printPlayerHeader();

    int count = 0;

    while (temp != NULL && count < k)
    {
        printPlayer(temp->player);

        temp = temp->nextRole;

        count++;
    }

    if (count == 0)
    {
        printf("No players found for this role.\n");
    }

    printf("============================================================================================\n");
}


/* Max heapify */

void heapify(HeapNode heap[], int n, int i)
{
    int largest = i;

    int left = 2 * i + 1;

    int right = 2 * i + 2;

    if (left < n &&
        heap[left].node->player.PerformanceIndex >
        heap[largest].node->player.PerformanceIndex)
    {
        largest = left;
    }

    if (right < n &&
        heap[right].node->player.PerformanceIndex >
        heap[largest].node->player.PerformanceIndex)
    {
        largest = right;
    }

    if (largest != i)
    {
        HeapNode temp = heap[i];

        heap[i] = heap[largest];

        heap[largest] = temp;

        heapify(heap, n, largest);
    }
}


/* Insert into max heap */

void insertHeap(HeapNode heap[], int *size, HeapNode value)
{
    int i = *size;

    heap[i] = value;

    (*size)++;

    while (i > 0)
    {
        int parent = (i - 1) / 2;

        if (heap[parent].node->player.PerformanceIndex >=
            heap[i].node->player.PerformanceIndex)
        {
            break;
        }

        HeapNode temp = heap[parent];

        heap[parent] = heap[i];

        heap[i] = temp;

        i = parent;
    }
}


/* Remove maximum element from heap */

HeapNode removeHeap(HeapNode heap[], int *size)
{
    HeapNode result = heap[0];

    heap[0] = heap[*size - 1];

    (*size)--;

    if (*size > 0)
    {
        heapify(heap, *size, 0);
    }

    return result;
}


/* Display all players of a specific role across teams */

void displayAllPlayersByRole()
{
    int role;

    printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    scanf("%d", &role);

    if (role < 1 || role > 3)
    {
        printf("Invalid role!\n");
        return;
    }

    HeapNode heap[TEAM_COUNT];

    int heapSize = 0;

    int roleIndex = role - 1;

    /* Insert first player of every team's role list */

    for (int i = 0; i < TEAM_COUNT; i++)
    {
        if (teamList[i].roleHead[roleIndex] != NULL)
        {
            HeapNode newNode;

            newNode.node = teamList[i].roleHead[roleIndex];

            newNode.teamIndex = i;

            insertHeap(heap, &heapSize, newNode);
        }
    }

    printf("\n%s of All Teams:\n", getRoleName(role));

    printf("============================================================================================\n");

    printf("%-6s %-25s %-17s %-13s %-8s %-8s %-8s %-8s %-10s\n",
           "ID", "Name", "Team", "Role", "Runs", "Avg",
           "SR", "Wkts", "Perf.Index");

    printf("============================================================================================\n");

    int count = 0;

    /* Merge sorted lists using max heap */

    while (heapSize > 0)
    {
        HeapNode current = removeHeap(heap, &heapSize);

        ODIPlayer p = current.node->player;

        printf("%-6d %-25s %-17s %-13s %-8d %-8.2f %-8.2f %-8d %-10.2f\n",
               p.PlayerId,
               p.Name,
               p.TeamName,
               getRoleName(p.Role),
               p.TotalRuns,
               p.BattingAverage,
               p.StrikeRate,
               p.Wickets,
               p.PerformanceIndex);

        count++;

        /* Insert next player from the same team's list */

        if (current.node->nextRole != NULL)
        {
            HeapNode newNode;

            newNode.node = current.node->nextRole;

            newNode.teamIndex = current.teamIndex;

            insertHeap(heap, &heapSize, newNode);
        }
    }

    printf("============================================================================================\n");

    printf("Total Players: %d\n", count);
}


/* Free all allocated memory */

void freeMemory()
{
    for (int i = 0; i < TEAM_COUNT; i++)
    {
        PlayerNode *temp = teamList[i].allPlayers;

        while (temp != NULL)
        {
            PlayerNode *next = temp->nextAll;

            free(temp);

            temp = next;
        }

        teamList[i].allPlayers = NULL;
    }
}


/* Main function */

int main()
{
    initializeTeams();

    initializePlayers();

    int choice;

    while (1)
    {
        printf("\n");
        printf("============================================================\n");
        printf("       ICC ODI PLAYER PERFORMANCE ANALYZER\n");
        printf("============================================================\n");

        printf("1. Add Player to Team\n");
        printf("2. Display Players of a Specific Team\n");
        printf("3. Display Teams by Average Batting Strike Rate\n");
        printf("4. Display Top K Players of a Specific Team by Role\n");
        printf("5. Display All Players of Specific Role Across All Teams\n");
        printf("6. Exit\n");

        printf("============================================================\n");

        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input!\n");

            while (getchar() != '\n');

            continue;
        }

        switch (choice)
        {
            case 1:
                addPlayer();
                break;

            case 2:
                displayTeamPlayers();
                break;

            case 3:
                displayTeamsByStrikeRate();
                break;

            case 4:
                displayTopKPlayers();
                break;

            case 5:
                displayAllPlayersByRole();
                break;

            case 6:
                freeMemory();

                printf("Memory released. Exiting program...\n");

                return 0;

            default:
                printf("Invalid choice! Try again.\n");
        }
    }

    return 0;
}