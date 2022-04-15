#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include "cards.h"
#include "logic.h"
#include "interface.h"
#include "fileMg.h"

const char *CARD_FORMAT = "(%d, %d, %c)\n";
FILE *saveGame;

void save() {
    saveGame = fopen("save.dat", "w");
    fprintf(saveGame, "%d\n", begCardC);
    fprintf(saveGame, "%d\n", cardsC);
    fprintf(saveGame, "%d %d\n", myPoints, oppPoints);
    fprintf(saveGame, "%c\n", turn);
    for(int i = 0; i < begCardC; i++) {
        char state;
        if(strcmp(cardArr[i]->state, "inactive") == 0) state = 'i';
        else state = 'a';
        fprintf(saveGame, CARD_FORMAT, cardArr[i]->ID, cardArr[i]->iconId, state);
    }
    fclose(saveGame);
}

void loadGame() {
    saveGame = fopen("save.dat", "r");
    fscanf(saveGame, "%d\n", &begCardC);
    fscanf(saveGame, "%d\n", &cardsC);
    if(myId == 'A') fscanf(saveGame, "%d %d\n", &myPoints, &oppPoints);
    else fscanf(saveGame, "%d %d\n", &oppPoints, &myPoints);
    fscanf(saveGame, "%c\n", &turn);
    int id, iconId;
    char state;
    for(int i = 0; i < begCardC; i++) {
        fscanf(saveGame, CARD_FORMAT, &id, &iconId, &state);
        cardArr[i] = newCard(id, iconId);
        if(state == 'i') cardArr[i]->state = "inactive";
        cardArr[i]->iconName = iconNameArr[iconId];
    }
    gtk_container_foreach(GTK_CONTAINER(grid), (void*)gtk_widget_destroy, NULL);
    renderButtons();
    gtk_widget_show_all(window);
    fclose(saveGame);
}
