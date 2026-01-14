#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// data strucutres

typedef struct Player {
    int id;
    char name[30];
    int age;

    int wins;
    int losses;
    int consecutiveWins;
    int consecutiveLosses;

    struct Player* next;
} Player;

typedef struct {
    int size;
    Player* Head;   
    Player* Tail; 
    
} Queue;

typedef struct {
    int size;
    Player* head;
    
} List;



void initQueue(Queue* q) {
    q->Head = q->Tail = NULL;
    q->size = 0;
}

void enqueue(Queue* q, Player* p) {
    p->next = NULL;
    if (q->Tail == NULL) {
        q->Head = q->Tail = p;
    } else {
        q->Tail->next = p;
        q->Tail = p;
    }
    q->size++;
}

Player* dequeue(Queue* q) {
    if (q->Head == NULL) return NULL;

    Player* temp = q->Head;
    q->Head = q->Head->next;

    if (q->Head == NULL)
        q->Tail = NULL;

    temp->next = NULL;
    q->size--;
    return temp;
}


int sumDigits(int x) {
    int sum = 0;
    while (x > 0) {
        sum += x % 10;
        x /= 10;
    }
    return sum;
}

// function to give a random integer and generate score
int generateScore() {
    int value = rand() % 1000000;
    if (sumDigits(value) % 5 == 0)
        return 1;
    else 
        return 0;
}

Player* createPlayer(int id, const char* name, int age) {
    Player* p = (Player*)malloc(sizeof(Player));
    p->id = id;
    strcpy(p->name, name);
    p->age = age;

    p->wins = 0;
    p->losses = 0;
    p->consecutiveWins = 0;
    p->consecutiveLosses = 0;
    p->next = NULL;

    return p;
}


// round counter
int roundNumber = 1;

// PRIORITY SELECTION 

Player* selectPlayer(Queue* F1, Queue* F, Queue* F3) {
    if (F1->size > 0) return dequeue(F1);
    if (F->size > 0)  return dequeue(F);
    if (F3->size > 0) return dequeue(F3);
    return NULL;
}



Player* playRound(Player* p1, Player* p2,
                  Queue* F, Queue* F1, Queue* F3,
                  Queue* LP, Queue* LG) {

    int score1 = 0, score2 = 0, turns = 0;

    while (abs(score1 - score2) < 3 && turns < 12) {
        score1 += generateScore();
        score2 += generateScore();
        turns++;
    }

    printf("\nGame %d\n", roundNumber++);
    printf("Player (%d) %s : %d points\n", p1->id, p1->name, score1);
    printf("Player (%d) %s : %d points\n", p2->id, p2->name, score2);

    // if player 1 wins 
    if (score1 > score2) {

        p1->wins++;
        p1->consecutiveWins++;
        p1->consecutiveLosses = 0;

        p2->losses++;
        p2->consecutiveLosses++;
        p2->consecutiveWins = 0;

        if (p1->consecutiveWins == 3)
            enqueue(F1, p1);

        if (p2->losses == 5)
            enqueue(LP, p2);
        else if (p2->consecutiveLosses == 3)
            enqueue(F3, p2);
        else
            enqueue(F, p2);

        if (p1->wins == 5)
            enqueue(LG, p1);

        return p1;
    }

    // if player 2 wins
    else if (score2 > score1) {

        p2->wins++;
        p2->consecutiveWins++;
        p2->consecutiveLosses = 0;

        p1->losses++;
        p1->consecutiveLosses++;
        p1->consecutiveWins = 0;

        if (p2->consecutiveWins == 3)
            enqueue(F1, p2);

        if (p1->losses == 5)
            enqueue(LP, p1);
        else if (p1->consecutiveLosses == 3)
            enqueue(F3, p1);
        else
            enqueue(F, p1);

        if (p2->wins == 5)
            enqueue(LG, p2);

        return p2;
    }

    // else both returned to tail of F
    else {
        enqueue(F, p1);
        enqueue(F, p2);
        return NULL;
    }
}



void partOneGame(Queue* F, Queue* F1, Queue* F3,
                 Queue* LP, Queue* LG) {

    Player* currentWinner = NULL;

    while (F->size + F1->size + F3->size >= 2) {

        if (currentWinner == NULL)
            currentWinner = selectPlayer(F1, F, F3);

        Player* challenger = selectPlayer(F1, F, F3);
        if (!challenger) break;

        currentWinner = playRound(
            currentWinner, challenger,
            F, F1, F3, LP, LG
        );
    }
}



int main() {
    srand(time(NULL));

    Queue F, F1, F3, LP, LG;
    initQueue(&F);
    initQueue(&F1);
    initQueue(&F3);
    initQueue(&LP);
    initQueue(&LG);

    // creating players
    enqueue(&F, createPlayer(1, "Alice", 20));
    enqueue(&F, createPlayer(2, "Bob", 21));
    enqueue(&F, createPlayer(3, "Charlie", 22));
    enqueue(&F, createPlayer(4, "Diana", 23));
    enqueue(&F, createPlayer(5, "Eve", 24));

    partOneGame(&F, &F1, &F3, &LP, &LG);

    printf("\n===== END OF PART I =====\n");
    printf("Winners list (LG): %d players\n", LG.size);
    printf("Losers list (LP): %d players\n", LP.size);

    return 0;
}
