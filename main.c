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
static GtkWidget *window, *buttons[16], *grid;
static PipesPtr stream;
static char myId = 'B', yourId = 'A';
int begCardC;
int cardsC = 0;
char turn = 'A';

CardPtr selected1 = NULL, selected2 = NULL;
int selectedCount = 0;

char *CARD_FORMAT = "(%d, %d, %c)\n";
FILE *saveGame;

void alert(char *message);
static void quit();
static void selectCard(GtkWidget *button, CardPtr card);
static void forfeit(char *id);
static void check(GtkWidget *widget);
static void generateCards();
void saveQuit();
void loadGame();
void startFromSave();
void renderButtons();
void gameStart();
void updateCardsC(GtkWidget *widget, GtkWidget *text);
static gboolean listenForInst(gpointer data);
static void sendToPipe(char instr[6]);
static gboolean disableButtons();
void mainMenu();

int main(int argc, char *argv[])
{
    if((stream = initPipes(argc, argv)) == NULL) return 1;
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
    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_container_add(GTK_CONTAINER(window), grid);

    mainMenu();


    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}

void alert(char *message) {
    GtkWidget *dialog;
    dialog=gtk_message_dialog_new (GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,
				   GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,"%s",message);
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
}

static void quit() {
    closePipes(stream);
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
    if(turn == myId) {
        char inst[6];
        sprintf(inst, "SE %d", card->ID);
        sendToPipe(inst);
        if(turn == 'A') turn = 'B';
                else turn = 'A';
    }
}

static void forfeit(char *id) {
    char winner[28];
    sprintf(winner, "Player %c has won the game!!!", *id);
    alert(winner);
    mainMenu();
}

static void check(GtkWidget *widget) {
    if(selectedCount == 2) {
        if(selected1->iconId == selected2->iconId) {

            selected1->state = "inactive";
            selected2->state = "inactive";
            gtk_widget_set_sensitive(GTK_WIDGET(buttons[selected1->ID]), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(buttons[selected2->ID]), FALSE);
            selectedCount = 0;
            g_print("PAIRED CARDS: %d %d\n", selected1->ID, selected2->ID);

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
        forfeit(&myId);
        sendToPipe("WINNR");
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

void save() {
    saveGame = fopen("save.dat", "w");
    fprintf(saveGame, "%d\n", begCardC);
    fprintf(saveGame, "%d\n", cardsC);
    for(int i = 0; i < begCardC; i++) {
        char state;
        if(strcmp(cardArr[i]->state, "inactive") == 0) state = 'i';
        else state = 'a';
        fprintf(saveGame, CARD_FORMAT, cardArr[i]->ID, cardArr[i]->iconId, state);
    }
    fclose(saveGame);
}

void saveQuit() {
    save();
    quit();
}

void loadGame() {
    saveGame = fopen("save.dat", "r");
    fscanf(saveGame, "%d\n", &begCardC);
    fscanf(saveGame, "%d\n", &cardsC);


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

void startFromSave() {
    loadGame();
    sendToPipe("START");
}

void renderButtons() {
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
        g_signal_connect(G_OBJECT(buttons[i]), "leave", G_CALLBACK(check), grid);

    }

    GtkWidget *button = gtk_button_new_with_label("Forfeit");
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(forfeit), &yourId);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 3, begCardC/4,1);

    button = gtk_button_new_with_label("Save & quit");
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(saveQuit), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, (begCardC % 4 == 0 ? begCardC/4: begCardC/4 + 1), 3, begCardC/4,1);

}

void gameStart() {
    if(cardsC == 0) {alert("Please enter a valid number of card pairs");}
    else {
        begCardC = cardsC;
        for(int i = 0; i < 8; i++) iconIdArr[i] = 0;

        gtk_container_foreach(GTK_CONTAINER(grid), (void*)gtk_widget_destroy, NULL);
        generateCards();

        FILE *cards;
        cards = fopen("save.dat", "w");
        fprintf(saveGame, "%d\n", begCardC);
        fprintf(saveGame, "%d\n", begCardC);
        for(int i = 0; i < begCardC; i++) {
            fprintf(cards, CARD_FORMAT, cardArr[i]->ID, cardArr[i]->iconId, (strcmp(cardArr[i]->state, "inactive") == 0 ? 'i' : 'a'));
        }
        fclose(cards);

        sendToPipe("START");

        renderButtons();
        gtk_widget_show_all(window);


    }
}

void updateCardsC(GtkWidget *widget, GtkWidget *text) {
    int temp;
    sscanf(gtk_entry_get_text(GTK_ENTRY(text)), "%d", &temp);
    if(temp >= 2 && temp <= 8) {
        char str[39];
        sprintf(str, "Successfully updated cards number to %d.", temp * 2);
        cardsC = temp * 2;
        alert(str);
    } else {
        alert("Number must be in range of 2 to 8.");
    }
}

static gboolean listenForInst(gpointer data) {
    gchar in[6] = "";
    if(getStringFromPipe(stream, in + strlen(in), 6)) {
        if(strcmp(in, "START") == 0) {
            loadGame();
        } else if(strcmp(in, "WINNR") == 0) {
            mainMenu();
            forfeit(&yourId);
        }
        else {
            char inst[6]; int arg;
            sscanf(in, "%s %d", inst, &arg);
            if(strcmp(inst, "SE") == 0) {
                selectCard(buttons[arg], cardArr[arg]);
                if(turn == 'A') turn = 'B';
                else turn = 'A';
            }
        }
    }
    return TRUE;
}

static void sendToPipe(char instr[6]) {
    sendStringToPipe(stream, instr);
}

static gboolean disableButtons() {
    if(turn != myId) {
        for(int i = 0; i < begCardC; i++) {
            gtk_widget_set_sensitive(buttons[i], FALSE);
        }
    } else {
        for(int i = 0; i < begCardC; i++) {
            if(strcmp(cardArr[i]->state, "inactive") != 0) {
                gtk_widget_set_sensitive(buttons[i], TRUE);
            }
        }
    }
    return TRUE;
}

void mainMenu() {
    gtk_container_foreach(GTK_CONTAINER(grid), (void*)gtk_widget_destroy, NULL);

    GtkWidget *label = gtk_label_new("MEMORY GAME");
    gtk_grid_attach(GTK_GRID(grid), label, 0,0,1,1);


    label = gtk_label_new("Enter a number of card pairs you want to play with (2 - 8): ");
    gtk_grid_attach(GTK_GRID(grid), label, 0,1,1,1);

    GtkWidget *text = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(text), "");
    g_signal_connect(G_OBJECT(text), "activate", G_CALLBACK(updateCardsC),(gpointer) text);
    gtk_grid_attach(GTK_GRID(grid), text, 0,2,1,1);


    GtkWidget *button = gtk_button_new_with_label("New Game");
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(gameStart), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 3, 1, 1);

    if(myId == 'B') {
        gtk_widget_set_sensitive(button, FALSE);
        gtk_widget_set_sensitive(text, FALSE);
    }

    saveGame = fopen("save.dat", "r");

    button = gtk_button_new_with_label("Continue");
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(startFromSave), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 4, 1, 1);
    if(saveGame == NULL) gtk_widget_set_sensitive(button, FALSE);

    fclose(saveGame);

    if(myId == 'B') {
        gtk_widget_set_sensitive(button, FALSE);
    }

    button = gtk_button_new_with_label("Quit");
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(quit), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 5, 1, 1);

    gtk_widget_show_all(window);

    g_timeout_add(100, listenForInst, NULL);
    g_timeout_add(100, disableButtons, NULL);
}


