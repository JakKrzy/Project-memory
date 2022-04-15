#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <gtk/gtk.h>
#include "cards.h"
#include "interface.h"
#include "fileMg.h"
#include "logic.h"

char myId = 'B', yourId = 'A';
int myPoints, oppPoints;
char turn = 'A';
int selectedCount = 0;
GtkWidget *buttons[16], *myPointsLabel, *oppPointsLabel;

void sendToPipe(char inst[6]);
void alert(char *message);

void selectCard(GtkWidget *button, CardPtr card) {

    if(selectedCount == 0 && strcmp(card->state, "active") == 0) {
        gtk_button_set_label(GTK_BUTTON(button), NULL);
        selected1 = card;
        selectedCount++;
        selected1->state = "selected";
        gtk_widget_set_sensitive(GTK_WIDGET(buttons[card->ID]), FALSE);
    }
    else if(selectedCount == 1 && strcmp(card->state, "active") == 0) {
        gtk_button_set_label(GTK_BUTTON(button), NULL);
        selected2 = card;
        selectedCount++;
        selected2->state = "selected";
    }
    if(turn == myId) {
        char inst[6];
        sprintf(inst, "SE %d", card->ID);
        sendToPipe(inst);
    }
}

void declareWinner() {
    if(myPoints > oppPoints) alert("You win!!!");
    else if(myPoints == oppPoints) alert("It's a draw!");
    else alert("Opponent wins!");
}

void check() {
    if(selectedCount == 2) {
        if(selected1->iconId == selected2->iconId) {
            selected1->state = "inactive";
            selected2->state = "inactive";
            gtk_widget_set_sensitive(GTK_WIDGET(buttons[selected1->ID]), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(buttons[selected2->ID]), FALSE);
            selectedCount = 0;
            cardsC -= 2;
            if(turn == myId) {
                myPoints++;
                gchar str[15];
                sprintf(str, "Your points: %d", myPoints);
                gtk_label_set_text(GTK_LABEL(myPointsLabel), str);
            } else {
                oppPoints++;
                gchar str[20];
                sprintf(str, "Opponent's points %d", oppPoints);
                gtk_label_set_text(GTK_LABEL(oppPointsLabel), str);
            }
        } else {
            Sleep(500);
            selectedCount = 0;
            selected1->state = "active";
            selected2->state = "active";
            gtk_button_set_label(GTK_BUTTON(buttons[selected1->ID]), " ");
            gtk_button_set_label(GTK_BUTTON(buttons[selected2->ID]), " ");
        }
        if(turn == 'A') turn = 'B';
        else turn = 'A';
    }
    if(cardsC == 0) {
        declareWinner();
        mainMenu();
    }
}

gboolean changeTurn() {
    if(selectedCount == 2) check();
    if(cardsC == 0) return FALSE;
    return TRUE;
}

gboolean disableButtons() {
    if(turn != myId) {
        for(int i = 0; i < begCardC; i++) gtk_widget_set_sensitive(buttons[i], FALSE);
    } else for(int i = 0; i < begCardC; i++) if(strcmp(cardArr[i]->state, "inactive") != 0) gtk_widget_set_sensitive(buttons[i], TRUE);
    if(cardsC == 0) return FALSE;
    return TRUE;
}

void gameStart() {
    if(cardsC == 0) alert("Please enter a valid number of card pairs");
    else {
        begCardC = cardsC;
        myPoints = 0; oppPoints = 0;
        for(int i = 0; i < 8; i++) iconIdArr[i] = 0;
        gtk_container_foreach(GTK_CONTAINER(grid), (void*)gtk_widget_destroy, NULL);
        generateCards();
        FILE *cards;
        cards = fopen("save.dat", "w");
        fprintf(cards, "%d\n", begCardC);
        fprintf(cards, "%d\n", begCardC);
        fprintf(cards, "0 0\n");
        fprintf(cards, "A\n");
        for(int i = 0; i < begCardC; i++) fprintf(cards, CARD_FORMAT, cardArr[i]->ID, cardArr[i]->iconId, (strcmp(cardArr[i]->state, "inactive") == 0 ? 'i' : 'a'));
        fclose(cards);

        sendToPipe("START");
        renderButtons();
        gtk_widget_show_all(window);
    }
}
