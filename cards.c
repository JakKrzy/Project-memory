#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <gtk/gtk.h>
#include "cards.h"

int iconIdArr[8] = {0};
char iconNameArr[8][15] = {
    "call-start", "edit-delete", "go-home", "list-add", "system-run",
    "zoom-in", "go-jump", "edit-redo",};
CardPtr cardArr[16];
int begCardC, cardsC = 16;
CardPtr selected1 = NULL, selected2 = NULL;

void alert(char *message);

CardPtr newCard(int ID,int iconId) {
    CardPtr card = malloc(sizeof(Card));
    card->ID = ID;
    card->iconId = iconId;
    card->state = "active";
    return card;
}

void generateCards() {
    srand(time(NULL));
    for(int i = 0; i < cardsC; i++) cardArr[i] = NULL;
    for(int i = 0; i < cardsC/2; i++) {
        while(iconIdArr[i] < 2) {
            int pos;
            do {
                pos = rand() % cardsC;
            } while (cardArr[pos] != NULL);
            cardArr[pos] = newCard(pos, i);
            cardArr[pos]->iconName = iconNameArr[i];
            iconIdArr[i]++;
        }
    }
}

void updateCardsC(GtkWidget *text) {
    int temp;
    sscanf(gtk_entry_get_text(GTK_ENTRY(text)), "%d", &temp);
    if(temp >= 2 && temp <= 8) {
        char str[39];
        sprintf(str, "Successfully updated cards number to %d.", temp * 2);
        cardsC = temp * 2;
        alert(str);
    } else alert("Number must be in range of 2 to 8.");
}
