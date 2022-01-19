#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include "fifo.h"

static struct card {
    int id, isInGame, iconId;
    char *state;
};


static GtkWidget *window;
static PipesPtr stream;
static char myId = 'B', yourId = 'A';
int cardsC = 0;

void alert(char *message) {
    GtkWidget *dialog;
    dialog=gtk_message_dialog_new (GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,
				   GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,"%s",message);
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
}

static void updateCardsC(GtkWidget *widget, GtkWidget *text) {
    int temp;
    sscanf(gtk_entry_get_text(GTK_ENTRY(text)), "%d", &temp);
    if(temp >= 2 && temp <= 8) {
        cardsC = temp * 2;
        alert("Successfully updated cards number.");
    } else {
        alert("Number must be in range of 2 to 8.");
    }
}

static void startTheGame(GtkWidget *widget, GtkWidget *grid) {
    alert("LET THE GAME BEGIN!");
}

static void quit(GtkWidget *widget, gpointer data) {
    //closePipes(stream);
    gtk_main_quit();
}


int main(int argc, char *argv[])
{
   // if((stream = initPipes(argc, argv)) == NULL) return 1;
    if(argc == 2 && strcmp(argv[1], "A") == 0) {
        myId = 'A'; yourId = 'B';
    }
    gtk_init(&argc, &argv);

    gchar header[17];
    sprintf(header, "Memory - Player %c", myId);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), header);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(exit), NULL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 15);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget *label = gtk_label_new("MEMORY GAME");
    gtk_grid_attach(GTK_GRID(grid), label, 0,0,1,1);

    if(myId == 'A') {
        label = gtk_label_new("Enter a number of card pairs you want to play with (2 - 8): ");
        gtk_grid_attach(GTK_GRID(grid), label, 0,1,1,1);

        GtkWidget *text = gtk_entry_new();
        gtk_entry_set_text(GTK_ENTRY(text), " ");
        g_signal_connect(G_OBJECT(text), "activate", G_CALLBACK(updateCardsC),(gpointer) text);
        gtk_grid_attach(GTK_GRID(grid), text, 0,2,1,1);

        GtkWidget *button = gtk_button_new_with_label("Start!");
        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(startTheGame), (gpointer) grid);
        gtk_grid_attach(GTK_GRID(grid), button, 0, 3, 1, 1);

        button = gtk_button_new_with_label("Quit");
        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(quit), NULL);
        gtk_grid_attach(GTK_GRID(grid), button, 0, 4, 1, 1);
    } else {
        label = gtk_label_new("Waiting for Player A");
        gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    }


    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}


