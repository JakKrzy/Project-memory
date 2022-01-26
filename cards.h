#include <stdlib.h>
#include <stdio.h>

typedef struct card {
    int ID, iconId;
    char *state, *iconName;
    // Possible states: active, selected, inactive
} Card;

typedef Card *CardPtr;

CardPtr newCard(int ID,int iconId);
