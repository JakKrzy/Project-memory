#include <stdlib.h>
#include <stdio.h>

typedef struct card {
    int ID, iconId;
    char *state, *iconName;
    // Possible states: active, selected, inactive
} Card;

typedef Card *CardPtr;

extern int iconIdArr[8];
extern char iconNameArr[8][15];
extern CardPtr cardArr[16];
extern int begCardC, cardsC;
extern CardPtr selected1, selected2;

CardPtr newCard(int ID,int iconId);
void generateCards();
void updateCardsC(GtkWidget *text);
