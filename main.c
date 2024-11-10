#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "card_system.h"
#include "menu.h"
#include "safeinput.h"

int main() {
    SystemState state = {NULL, 0, false}; 
    loadCardsFromFile(&state);

    while(true) {
        showMenu();
        int selection;
        GetInputInt("Action:", &selection);

        switch (selection) {
            case 1:
                printf("\n");
                remoteOpenDoor(&state);
                break;
            case 2:
                printf("\n");
                listAllCards(&state);
                break;
            case 3:
                printf("\n");
                addRemoveMenu(&state);
                break;
            case 4:
                free(state.cards);
                return 0;
            case 9:
                fakeCardScan(&state);
                break;
            default:
                printf("Invalid selection. Please try again.\n");
                break;
        }
    }
}