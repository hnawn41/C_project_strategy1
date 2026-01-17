#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Player and Queue structures */

typedef struct Player {
    int id;
    char name[30];
    int age;

    int score;
    int wins, losses;
    int consecutiveWins;
    int consecutiveLosses;

    int inFinalList;      /* 0 = playing, 1 = in LG or LP , to avoid repetition */

    struct Player* next;
} Player;

typedef struct {
    Player* head;
    Player* tail;
    int size;
} Queue;

Player* LG = NULL;   /* winners list (descending by score) */
Player* LP = NULL;   /* losers list */

/* Queue operations */

void initQueue(Queue* q) {
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
}

void enqueue(Queue* q, Player* p) {
    if (p == NULL) return;
    p->next = NULL;
    if (q->tail == NULL) {
        q->head = p;
        q->tail = p;
    } else {
        q->tail->next = p;
        q->tail = p;
    }
    q->size++;
}

Player* dequeue(Queue* q) {
    if (q->head == NULL) return NULL;

    Player* p = q->head;
    q->head = q->head->next;

    if (q->head == NULL)
        q->tail = NULL;

    p->next = NULL;
    q->size--;
    return p;
}

/* List operations */

void insertLP(Player* p) {
    if (p == NULL || p->inFinalList == 1) return;
    p->inFinalList = 1;
    p->next = LP;
    LP = p;
}

void insertLG(Player* p) {
    if (p == NULL || p->inFinalList == 1) return;
    p->inFinalList = 1;

    if (!LG || p->score > LG->score) {
        p->next = LG;
        LG = p;
        return;
    }

    Player* cur = LG;
    while (cur->next && cur->next->score >= p->score)
        cur = cur->next;

    p->next = cur->next;
    cur->next = p;
}

/* Scoring functions */

int sumDigits(int x) {
    int s = 0;
    while (x > 0) {
        s += x % 10;
        x /= 10;
    }
    return s;
}

int scorePart1() {
    int v = rand() % 1000000;
    return (sumDigits(v) % 5 == 0);
}

int gcd(int a, int b) {
    while (b > 0) {
        int t = b;
        b = a % b;
        a = t;
    }
    return a;
}

int containsDigit(int n, int d) {
    while (n > 0) {
        if (n % 10 == d) return 1;
        n /= 10;
    }
    return 0;
}

int scorePart2() {
    int a = rand() % 1000 + 1;
    int b = rand() % 1000 + 1;
    int g = gcd(a, b);

    while (g > 0) {
        int d = g % 10;
        if (containsDigit(a, d) || containsDigit(b, d))
            return 1;
        g /= 10;
    }
    return 0;
}

/* Player selection */

Player* selectPlayer(Queue* F1, Queue* F, Queue* F3) {
    if (F1->size > 0) return dequeue(F1);
    if (F->size  > 0) return dequeue(F);
    if (F3->size > 0) return dequeue(F3);
    return NULL;
}


Player* selectOpponent(Queue* F1, Queue* F, Queue* F3, Player* winner) {
    Player* candidate = NULL;

    if (F1->size > 0) {
        candidate = dequeue(F1);
        if (candidate == winner) {
            enqueue(F1, candidate);
            candidate = NULL;
        }
    }
    if (candidate == NULL && F->size > 0) {
        candidate = dequeue(F);
        if (candidate == winner) {
            enqueue(F, candidate);
            candidate = NULL;
        }
    }
    if (candidate == NULL && F3->size > 0) {
        candidate = dequeue(F3);
        if (candidate == winner) {
            enqueue(F3, candidate);
            candidate = NULL;
        }
    }
    return candidate;
}

/* State display */

void showQueue(const char* name, Queue* q) {
    printf("\n%s (size=%d):\n", name, q->size);
    if (q->head == NULL) {
        printf("  [empty]\n");
        return;
    }
    Player* p = q->head;
    while (p) {
        printf("  %s (Score: %d, Wins: %d, Loses: %d, Consecutive Wins: %d, Consecutive Losses: %d)\n",
               p->name, p->score, p->wins, p->losses, p->consecutiveWins, p->consecutiveLosses);
        p = p->next;
    }
}

void showList(const char* name, Player* list) {
    printf("\n%s:\n", name);
    if (!list) {
        printf("  [empty]\n");
        return;
    }
    Player* p = list;
    while (p) {
        printf("  %s (Score: %d, Wins: %d, Loses: %d, Consecutive Wins: %d, Consecutive Losses: %d)\n",
               p->name, p->score, p->wins, p->losses, p->consecutiveWins, p->consecutiveLosses);
        p = p->next;
    }
}

void showState(Queue* F, Queue* F1, Queue* F3) {
    showQueue("Queue F", F);
    showQueue("Queue F1", F1);
    showQueue("Queue F3", F3);
    showList("List LG", LG);
    showList("List LP", LP);
    printf("\n-------------------\n");
}

/* Play a round */

Player* playRound(Player* p1, Player* p2,
                  Queue* F, Queue* F1, Queue* F3,
                  int part, int roundNumber) {

    int s1 = 0, s2 = 0, turns = 0;
    time_t start = time(NULL);

    printf("\n==============================\n");
    printf("ROUND %d - Part %d | Start: %s", roundNumber, part, ctime(&start));
    printf("%s vs %s\n", p1->name, p2->name);
    printf("------------------------------\n");

    if (part == 1) {
        while (abs(s1 - s2) < 3 && turns < 12) {
            s1 += scorePart1();
            s2 += scorePart1();
            turns++;
        }
    } else {
        while (abs(s1 - s2) < 3 && turns < 16) {
            s1 += scorePart2();
            s2 += scorePart2();
            turns++;
        }
    }

    printf("Score: %s = %d | %s = %d\n", p1->name, s1, p2->name, s2);

    p1->score += s1;
    p2->score += s2;

    if (s1 == s2) {
        printf("Result: DRAW\n");
        if (!p1->inFinalList) enqueue(F, p1);
        if (!p2->inFinalList) enqueue(F, p2);
        showState(F, F1, F3);
        return NULL;
    }

    Player* win = (s1 > s2) ? p1 : p2;
    Player* lose = (s1 > s2) ? p2 : p1;

    printf("Winner: %s | Loser: %s\n", win->name, lose->name);

    win->wins++;
    lose->losses++;
    win->consecutiveWins++;
    lose->consecutiveLosses++;
    win->consecutiveLosses = 0;
    lose->consecutiveWins = 0;

    if (part == 1) {
        if (win->consecutiveWins == 3 && !win->inFinalList)
            enqueue(F1, win);

        if (lose->losses == 5)
            insertLP(lose);
        else if (lose->consecutiveLosses == 3 && !lose->inFinalList)
            enqueue(F3, lose);
        else if (!lose->inFinalList)
            enqueue(F, lose);

        if (win->wins == 5)
            insertLG(win);
    } else {
        if (lose->losses >= 2)
            insertLP(lose);
        else if (!lose->inFinalList)
            enqueue(F3, lose);

        if (!win->inFinalList) enqueue(F1, win);

        if (win->consecutiveWins >= 2)
            insertLG(win);
    }

    time_t end = time(NULL);
    printf("End of Round: %s", ctime(&end));

    showState(F, F1, F3);

    return win;
}

/* Run the game */

void runGame(Queue* F, Queue* F1, Queue* F3) {
    int n = F->size;
    Player* winner = NULL;
    int roundNumber = 1;

    /* Part 1 */
    for (int i = 0; i < 3 * n && F->size + F1->size + F3->size >= 2; i++) {
        if (!winner)
            winner = selectPlayer(F1, F, F3);

        Player* c = selectOpponent(F1, F, F3, winner);
        if (!c) break;

        winner = playRound(winner, c, F, F1, F3, 1, roundNumber++);
    }

    /* Part 2 */
    for (int i = 0; i < 2 * n && F->size + F1->size + F3->size >= 2; i++) {
        if (!winner)
            winner = selectPlayer(F1, F, F3);

        Player* c = selectOpponent(F1, F, F3, winner);
        if (!c) break;

        winner = playRound(winner, c, F, F1, F3, 2, roundNumber++);
    }

    /* Place remaining players in final lists */
    while (F1->size) insertLG(dequeue(F1));
    while (F->size)  insertLP(dequeue(F));
    while (F3->size) insertLP(dequeue(F3));
}

/* Show top 3 winners */

void showTop3() {
    printf("\nTOP 3 WINNERS\n");
    Player* p = LG;
    int i = 1;

    while (p && i <= 3) {
        printf("%d) %s - %d pts\n", i, p->name, p->score);
        p = p->next;
        i++;
    }
}

/* Show players with 0 wins and summary */

void showSummary(Player* players[], int n) {
    printf("\nPLAYER WIN/LOSS SUMMARY\n");

    printf("Players with 0 wins:\n");
    for (int i = 0; i < n; i++)
        if (players[i]->wins == 0)
            printf("  %s\n", players[i]->name);

    printf("\nPlayers with 1,2,3 wins:\n");
    for (int i = 0; i < n; i++)
        if (players[i]->wins >=1 && players[i]->wins <=3)
            printf("  %s - Wins: %d\n", players[i]->name, players[i]->wins);

    printf("\nPlayers with 1,2,3 losses:\n");
    for (int i = 0; i < n; i++)
        if (players[i]->losses >=1 && players[i]->losses <=3)
            printf("  %s - Losses: %d\n", players[i]->name, players[i]->losses);
}

/* Main function */

int main() {
    srand(time(NULL));

    Queue F, F1, F3;
    initQueue(&F);
    initQueue(&F1);
    initQueue(&F3);

    int numPlayers;
    printf("Enter number of players ( >= 2 ): ");
    scanf("%d", &numPlayers);

    Player** allPlayers = (Player**)malloc(numPlayers * sizeof(Player*));
    if (allPlayers == NULL) {
    printf("Memory allocation failed!\n");
    return 1;
}
 // keep track for summary

    for (int i = 0; i < numPlayers; i++) {
        char name[30];
        int age;

        printf("Enter name of player %d: ", i+1);
        scanf("%s", name);
        printf("Enter age of player %d: ", i+1);
        scanf("%d", &age);

        Player* p = (Player*)malloc(sizeof(Player));
        p->id = i+1;
        strcpy(p->name, name);
        p->age = age;
        p->score = 0;
        p->wins = p->losses = 0;
        p->consecutiveWins = p->consecutiveLosses = 0;
        p->inFinalList = 0;
        p->next = NULL;

        enqueue(&F, p);
        allPlayers[i] = p;
    }

    runGame(&F, &F1, &F3);
    showTop3();
    showSummary(allPlayers, numPlayers);

    /* free all players */
    Player* tmp;
    while (LG) { tmp = LG; LG = LG->next; free(tmp); }
    while (LP) { tmp = LP; LP = LP->next; free(tmp); }

    return 0;
}
