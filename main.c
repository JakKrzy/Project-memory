#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "fifo.h"
#include "cards.h"


int iconIdArr[8] = {0};
const char iconNameArr[8][15] = {
    "call-start", "edit-delete", "go-home", "list-add", "system-run",
    "zoom-in", "go-jump", "edit-redo",};
CardPtr cardArr[16];
static GtkWidget *window, *buttons[16];
//static PipesPtr stream;
static char myId = 'B', yourId = 'A';
int pointsA = 0;
GtkWidget *points;
int cardsC = 16;

CardPtr selected1 = NULL, selected2 = NULL;
int selectedCount = 0;

void alert(char *message) {
    GtkWidget *dialog;
    dialog=gtk_message_dialog_new (GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,
				   GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,"%s",message);
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
}

static void quit() {
    //closePipes(stream);
    gtk_main_quit();
}

static void selectCard(GtkWidget *button, CardPtr card) {
    gchar label[1];

    if(selectedCount == 0 && strcmp(card->state, "active") == 0) {
        sprintf(label, "%d", card->iconId);
        gtk_button_set_label(GTK_BUTTON(button), NULL);
        selected1 = card;
        selectedCount++;
        selected1->state = "selected";
        g_print("Selected card id: %d, icon: %d\n\n", selected1->ID,selected1->iconId);
    }
    else if(selectedCount == 1 && strcmp(card->state, "selected") == 0) {
        g_print("Unselected card id: %d, icon: %d\n\n", selected1->ID,selected1->iconId);
        sprintf(label, " ");
        selected1->state = "active";
        gtk_button_set_label(GTK_BUTTON(button), label);
        selectedCount--;
    }
    else if(selectedCount == 1 && strcmp(card->state, "active") == 0) {
        sprintf(label, "%d", card->iconId);
        gtk_button_set_label(GTK_BUTTON(button), NULL);
        selected2 = card;
        selectedCount++;
        selected2->state = "selected";
        g_print("Selected card id: %d, icon: %d\n\n", selected2->ID,selected2->iconId);
    }
}

/* static void forfeit(GtkWidget *widget, gpointer data) {
   char winner[28];
    sprintf(winner, "Player %c has won the game!!!", yourId);
    alert(winner);
} */

static void check(GtkWidget *widget, GtkWidget *grid) {
    if(selectedCount == 2) {
        if(selected1->iconId == selected2->iconId) {
            pointsA++;
            gchar str[10];
            sprintf(str, "Points: %d", pointsA);
            gtk_label_set_label(GTK_LABEL(points), str);

            selected1->state = "inactive";
            selected2->state = "inactive";
            gtk_widget_set_sensitive(GTK_WIDGET(buttons[selected1->ID]), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(buttons[selected2->ID]), FALSE);
            selectedCount = 0;
            g_print("PAIRED CARDS: %d %d", selected1->ID, selected2->ID);

            cardsC -= 2;
        } else {
            Sleep(500);
            selectedCount = 0;
            selected1->state = "active";
            selected2->state = "active";
            gtk_button_set_label(GTK_BUTTON(buttons[selected1->ID]), " ");
            gtk_button_set_label(GTK_BUTTON(buttons[selected2->ID]), " ");
        }
    }
    if(cardsC == 0) {
        alert("You win!");
        quit();
    }
}

static void generateCards() {
    srand(time(NULL));
    for(int i = 0; i < cardsC; i++) { cardArr[i] = NULL; }
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

static void renderButtons(GtkWidget *grid) {
    for(int i = 0; i < cardsC; i++) { // Rendering cards as buttons
        GtkWidget *image;
        gchar iconName[16];
        sprintf(iconName, "%s", cardArr[i]->iconName);
        image = gtk_image_new_from_icon_name(iconName, GTK_ICON_SIZE_BUTTON);
        buttons[i] = gtk_button_new_with_label(" ");
        gtk_button_set_image(GTK_BUTTON(buttons[i]), image);
        if(i < cardsC/2) gtk_grid_attach(GTK_GRID(grid), buttons[i], i, 1, 1,1);
        else gtk_grid_attach(GTK_GRID(grid), buttons[i], i - (cardsC/2), 2, 1,1);
        g_signal_connect(G_OBJECT(buttons[i]), "clicked", G_CALLBACK(selectCard), (CardPtr) cardArr[i]);
        g_signal_connect(G_OBJECT(buttons[i]), "leave", G_CALLBACK(check), grid);
    }
}

static void gameStart(GtkWidget *grid) {
    generateCards();
    gchar str[10];
    sprintf(str, "Points: %d", pointsA);
    points = gtk_label_new(str);
    gtk_grid_attach(GTK_GRID(grid), points, 0, 0,1,1);
    renderButtons(grid);
}

int main(int argc, char *argv[])
{
    //if((stream = initPipes(argc, argv)) == NULL) return 1;
    if(argc == 2 && strcmp(argv[1], "A") == 0) {
        myId = 'A'; yourId = 'B';
    }

    gtk_init(&argc, &argv);

    gchar header[17];
    sprintf(header, "Memory - Player %c", myId);
    // Window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), header);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(exit), NULL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 15);
    // Grid
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_container_add(GTK_CONTAINER(window), grid);

    gameStart(grid);

    // Footer buttons
    /*
    button = gtk_button_new_with_label("Forfeit");
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(forfeit), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 3, cardsC/4,1);

    button = gtk_button_new_with_label("Save & quit");
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(quit), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, cardsC/4, 3, cardsC/4,1);
    */

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}


