#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "fifo.h"
#include "cards.h"


int iconIdArr[8] = {0};
CardPtr cardArr[16];
static GtkWidget *window;
//static PipesPtr stream;
static char myId = 'B', yourId = 'A';
int cardsC = 16;
char turn = 'A';
int selected1, selected2;

void alert(char *message) {
    GtkWidget *dialog;
    dialog=gtk_message_dialog_new (GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,
				   GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,"%s",message);
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
}

static void quit(GtkWidget *widget, gpointer data) {
    //closePipes(stream);
    gtk_main_quit();
}

static void selectCard(GtkWidget *widget, gpointer icon) {
    g_printf("Selected card icon: %d\n", icon);
}

static void forfeit(GtkWidget *widget, gpointer data) {
    char winner[28];
    sprintf(winner, "Player %c has won the game!!!", yourId);
    alert(winner);
}

static void save(GtkWidget *widget, gpointer data) {
    alert("Game saved!");
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
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget *button;

    srand(time(NULL));

    for(int i = 0; i < cardsC; i++) {
        cardArr[i] = NULL;
    }

    for(int i = 0; i < cardsC/2; i++) {
        while(iconIdArr[i] < 2) {
            int pos;
            do {
                pos = rand() % cardsC;
            } while (cardArr[pos] != NULL);
            cardArr[pos] = newCard(i);
            iconIdArr[i]++;
        }
    }

    for(int i = 0; i < cardsC; i++) {
        button = gtk_button_new();
        gtk_grid_attach(GTK_GRID(grid), button, i, 0, 1,1);
        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(selectCard), (gpointer) cardArr[i]->iconId);
    }

    button = gtk_button_new_with_label("Forfeit");
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(forfeit), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 1, 4,1);

    button = gtk_button_new_with_label("Save & quit");
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(save), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 4, 1, 4,1);

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}


