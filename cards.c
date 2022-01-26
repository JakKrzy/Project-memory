#include <stdlib.h>
#include <stdio.h>
#include "cards.h"

CardPtr newCard(int ID,int iconId) {
    CardPtr card = malloc(sizeof(Card));
    card->ID = ID;
    card->iconId = iconId;
    card->state = "active";
    return card;
}
