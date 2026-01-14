#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* data structures */

typedef struct Player {
    int id;
    char name[30];
    int age;

    int score;                 
    int wins, losses;
    int consecutiveWins;
    int consecutiveLosses;

    struct Player* next;
} Player;

typedef struct {
    Player* head;
    Player* tail;
    int size;
} Queue;


Player* LG = NULL;   // ordered descending by score
Player* LP = NULL;

/* queue operations */

void initQueue(Queue* q) {
    q->head = q->tail = NULL;
    q->size = 0;
}

void enqueue(Queue* q, Player* p) {
    p->next = NULL;
    if (q->tail == NULL) {
        q->head = q->tail = p;
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
    if (q->head == NULL) q->tail = NULL;
    p->next = NULL;
    q->size--;
    return p;
}

/* list operations */

void insertLP(Player* p) {
    p->next = LP;
    LP = p;
}

/* in descending order */
void insertLG(Player* p) {
    if (LG == NULL || p->score > LG->score) {
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

/*other needed functions*/

int sumDigits(int x) {
    int s = 0;
    while (x > 0) {
         s += x % 10; x /= 10; }
    return s;
}

/* part 1 scoring */
int scorePart1() {
    int v = rand() % 1000000;
    return (sumDigits(v) % 5 == 0);
}

/* part 2 scoring */
int gcd(int a, int b) {
    while (b > 0) { int t = b; b = a % b; a = t; }
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
        if (containsDigit(a, d) || containsDigit(b, d)) return 1;
        g /= 10;
    }
    return 0;
}

/* player selection */
Player* selectPlayer(Queue* F1, Queue* F, Queue* F3) {
    if (F1->size > 0) return dequeue(F1);
    if (F->size  > 0) return dequeue(F);
    if (F3->size > 0) return dequeue(F3);
    return NULL;
}

/* round  */
Player* playRound(Player* p1, Player* p2,
                  Queue* F, Queue* F1, Queue* F3,
                  int part) {

    int s1 = 0, s2 = 0, turns = 0;

    time_t start = time(NULL);
    printf("\n--- GAME %s ---\n", ctime(&start));
    printf("%s vs %s\n", p1->name, p2->name);

    if (part == 1) {
        /* part 1 : max 12 generations */
        while (abs(s1 - s2) < 3 && turns < 12) {
            s1 += scorePart1();
            s2 += scorePart1();
            turns++;
        }
    } else {
        /* part 2 : max 16 generations */
        while (abs(s1 - s2) < 3 && turns < 16) {
            s1 += scorePart2();
            s2 += scorePart2();
            turns++;
        }
    }

    printf("Score: %s=%d | %s=%d\n", p1->name, s1, p2->name, s2);

    p1->score += s1;
    p2->score += s2;

    if (s1 == s2) {
        enqueue(F, p1);
        enqueue(F, p2);
        return NULL;
    }

    Player* win;
    Player* lose;

    if (s1 > s2) {
        win = p1;
        lose = p2;
    } else {
        win = p2;
        lose = p1;
}

    win->wins++;
    lose->losses++;
    win->consecutiveWins++;
    lose->consecutiveLosses++;
    win->consecutiveLosses = 0;
    lose->consecutiveWins = 0;

    if (part == 1) {
        if (win->consecutiveWins == 3) enqueue(F1, win);
        if (lose->losses == 5) insertLP(lose);
        else if (lose->consecutiveLosses == 3) enqueue(F3, lose);
        else enqueue(F, lose);

        if (win->wins == 5) insertLG(win);
    } else {
        if (lose->losses >= 2) insertLP(lose);
        else enqueue(F3, lose);

        if (win->consecutiveWins >= 2) insertLG(win);
    }

    return win;
}

/* =========================================================
   GAME CONTROLLER
   ========================================================= */

void runGame(Queue* F, Queue* F1, Queue* F3) {
    int n = F->size;
    Player* winner = NULL;

    /* part 1 */
    for (int i = 0; i < 3 * n && F->size + F1->size + F3->size >= 2; i++) {
        if (winner == NULL) winner = selectPlayer(F1, F, F3);
        Player* c = selectPlayer(F1, F, F3);
        if (c == NULL) break;
        winner = playRound(winner, c, F, F1, F3, 1);
    }

    /* part 2 */
    for (int i = 0; i < 2 * n && F->size + F1->size + F3->size >= 2; i++) {
        if (winner == NULL) winner = selectPlayer(F1, F, F3);
        Player* c = selectPlayer(F1, F, F3);
        if (c == NULL) break;
        winner = playRound(winner, c, F, F1, F3, 2);
    }

    /* remaining players */
    while (F1->size) insertLG(dequeue(F1));
    while (F->size)  insertLP(dequeue(F));
    while (F3->size) insertLP(dequeue(F3));
}

/* display */
void showTop3() {
    printf("\n TOP 3 WINNERS \n");
    Player* p = LG;
    for (int i = 1; p != NULL && i <= 3; i++) {
        printf("%d) %s - %d pts\n", i, p->name, p->score);
        p = p->next;
    }
}

/* main */

int main() {
    srand(time(NULL));

    Queue F, F1, F3;
    initQueue(&F);
    initQueue(&F1);
    initQueue(&F3);

    /* Automatic players (NO keyboard input) */
    enqueue(&F, &(Player){1,"Alice",20,0,0,0,0,0,NULL});
    enqueue(&F, &(Player){2,"Bob",21,0,0,0,0,0,NULL});
    enqueue(&F, &(Player){3,"Charlie",22,0,0,0,0,0,NULL});
    enqueue(&F, &(Player){4,"Diana",23,0,0,0,0,0,NULL});
    enqueue(&F, &(Player){5,"Eve",24,0,0,0,0,0,NULL});

    runGame(&F, &F1, &F3);
    showTop3();

    return 0;
}
