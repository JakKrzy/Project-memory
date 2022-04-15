#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <windows.h>
#include "fifo.h"
#include "cards.h"
#include "logic.h"
#include "interface.h"
#include "fileMg.h"

PipesPtr stream;

void alert(char *message);
void forfeit();
void quit();
void startFromSave();
void saveQuit();
void sendToPipe(char instr[6]);
gboolean listenForInst();

int main(int argc, char *argv[])
{
    if((stream = initPipes(argc, argv)) == NULL) return 1;
    if(argc == 2 && strcmp(argv[1], "A") == 0) {
        myId = 'A'; yourId = 'B';
    }
    gtk_init(&argc, &argv);
    initWindowAndGrid();
    mainMenu();
    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
//CALLBACKS
void alert(char *message) {
    GtkWidget *dialog;
    dialog=gtk_message_dialog_new (GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,"%s",message);
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
}
void forfeit() {
    char winner[28];
    sendToPipe("LOSER");
    sprintf(winner, "Player %c has won the game!!!", yourId);
    cardsC = 0;
    alert(winner);
    mainMenu();
}
void quit() {
    sendToPipe("QUITT");
    closePipes(stream);
    gtk_main_quit();
}
void startFromSave() {
    loadGame();
    sendToPipe("START");
}
void saveQuit() {
    save();
    quit();
}
void sendToPipe(char instr[6]) {
    sendStringToPipe(stream, instr);
}
//LOGIC
gboolean listenForInst() {
    gchar in[6] = "";
    if(getStringFromPipe(stream, in + strlen(in), 6)) {
        if(strcmp(in, "START") == 0) loadGame();
        else if(strcmp(in, "LOSER") == 0) {
            cardsC = 0;
            alert("You win!!");
            mainMenu();
        } else if(strcmp(in, "QUITT") == 0) {
            closePipes(stream);
            gtk_main_quit();
        } else {
            char inst[6]; int arg;
            sscanf(in, "%s %d", inst, &arg);
            if(strcmp(inst, "SE") == 0) selectCard(buttons[arg], cardArr[arg]);
        }
    }
    if(cardsC == 0) return FALSE;
    return TRUE;
}
