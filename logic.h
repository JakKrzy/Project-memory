#include <stdio.h>
#include <stdlib.h>

extern char myId, yourId;
extern int myPoints, oppPoints;
extern char turn;
extern int selectedCount;
extern GtkWidget *buttons[16], *myPointsLabel, *oppPointsLabel;

void selectCard(GtkWidget *button, CardPtr card);
void declareWinner();
void check();
gboolean disableButtons();
gboolean changeTurn();
void gameStart();
