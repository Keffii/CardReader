#ifndef __MAIN_h
#define __MAIN_h

int main();
void showMenu();
void addRemoveMenu();

typedef struct {
    int cardNumber;
    bool hasAccess;
    char dateAdded[20];
}Card;

typedef struct{
    Card *cards;
    int cardCount;
    bool isDoorOpen;
    //Add new system states here if needed
}SystemState;

#endif