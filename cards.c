#include <stdlib.h>
#include <stdio.h>
#include "cards.h"

CardPtr newCard(int iconId) {
    CardPtr card = malloc(sizeof(Card));
    card->iconId = iconId;
    card->state = "active";
}
