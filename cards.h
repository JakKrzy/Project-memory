#include <stdlib.h>
#include <stdio.h>

typedef struct card {
    int iconId;
    char *state;
} Card;

typedef Card *CardPtr;

CardPtr newCard(int iconId);
