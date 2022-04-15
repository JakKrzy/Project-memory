#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "cards.h"
#include "logic.h"
#include "fileMg.h"
#include "interface.h"

GtkWidget *window, *grid;

void quit();
void forfeit();
void saveQuit();
void gameStart();
void startFromSave();
gboolean listenForInst();


void initWindowAndGrid() {
    gchar header[17];
    sprintf(header, "Memory - Player %c", myId);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), header);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(quit), NULL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 15);

    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_container_add(GTK_CONTAINER(window), grid);
}

void renderButtons() {
    gchar str1[15], str2[20]; sprintf(str1, "Your points: %d", myPoints); sprintf(str2, "Opponent's points: %d", oppPoints);
    myPointsLabel = gtk_label_new(str1); oppPointsLabel = gtk_label_new(str2);
    gtk_grid_attach(GTK_GRID(grid), myPointsLabel, 0,0,1,1); gtk_grid_attach(GTK_GRID(grid), oppPointsLabel, begCardC/2 - 1,0,1,1);
    for(int i = 0; i < begCardC; i++) {
        GtkWidget *image;
        gchar iconName[16];
        sprintf(iconName, "%s", cardArr[i]->iconName);
        image = gtk_image_new_from_icon_name(iconName, GTK_ICON_SIZE_BUTTON);
        if(strcmp(cardArr[i]->state, "inactive") == 0) {
            buttons[i] = gtk_button_new_with_label(NULL);
            gtk_widget_set_sensitive(buttons[i], FALSE);
        }
        else buttons[i] = gtk_button_new_with_label(" ");
        gtk_button_set_image(GTK_BUTTON(buttons[i]), image);
        if(i < begCardC/2) gtk_grid_attach(GTK_GRID(grid), buttons[i], i, 1, 1,1);
        else gtk_grid_attach(GTK_GRID(grid), buttons[i], i - (begCardC/2), 2, 1,1);
        g_signal_connect(G_OBJECT(buttons[i]), "clicked", G_CALLBACK(selectCard), (CardPtr) cardArr[i]);
    }
    GtkWidget *button = gtk_button_new_with_label("Forfeit");
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(forfeit), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 3, begCardC/4,1);
    if(myId == 'A') {
        button = gtk_button_new_with_label("Save & quit");
        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(saveQuit), NULL);
    } else {
        button = gtk_button_new_with_label("Quit");
        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(quit), NULL);
    }
    gtk_grid_attach(GTK_GRID(grid), button, (begCardC % 4 == 0 ? begCardC/4: begCardC/4 + 1), 3, begCardC/4,1);
    g_timeout_add(100, disableButtons, NULL);
    g_timeout_add(500, changeTurn, NULL);
}

void mainMenu() {
    gtk_container_foreach(GTK_CONTAINER(grid), (void*)gtk_widget_destroy, NULL);
    for(int i = 0; i < 16; i++) {
        if(i < 8) iconIdArr[i] = 0;
        buttons[i] = NULL;
    }
    cardsC = 16;
    turn = 'A';
    GtkWidget *label = gtk_label_new("MEMORY GAME");
    gtk_grid_attach(GTK_GRID(grid), label, 0,0,1,1);
    GtkWidget *button;
    if(myId == 'A') {
        label = gtk_label_new("Enter a number of card pairs you want to play with (2 - 8): ");
        gtk_grid_attach(GTK_GRID(grid), label, 0,1,1,1);

        GtkWidget *text = gtk_entry_new();
        gtk_entry_set_text(GTK_ENTRY(text), "");
        g_signal_connect(G_OBJECT(text), "activate", G_CALLBACK(updateCardsC),(gpointer) text);
        gtk_grid_attach(GTK_GRID(grid), text, 0,2,1,1);

        button = gtk_button_new_with_label("New Game");
        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(gameStart), NULL);
        gtk_grid_attach(GTK_GRID(grid), button, 0, 3, 1, 1);

        saveGame = fopen("save.dat", "r");
        button = gtk_button_new_with_label("Continue");
        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(startFromSave), NULL);
        gtk_grid_attach(GTK_GRID(grid), button, 0, 4, 1, 1);
        if(saveGame == NULL) gtk_widget_set_sensitive(button, FALSE);
        fclose(saveGame);
    }
    button = gtk_button_new_with_label("Quit");
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(quit), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 5, 1, 1);
    gtk_widget_show_all(window);
    g_timeout_add(100, listenForInst, NULL);
}
