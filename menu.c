#include <stdio.h>
#include "menu.h"
#include "safeinput.h"

void showMenu(void) {
    printf("\nAdmin menu:\n\n");
    printf("1. Remote open door\n");
    printf("2. List all cards in system\n");
    printf("3. Add/Remove access\n");
    printf("4. Exit\n");
    printf("9. FAKE TEST SCAN CARD\n");
}

void addRemoveMenu(SystemState* state) {
    while(true) {
        printf("1. Add/Remove Access\n");
        printf("2. Exit\n");

        int selection;
        int cardNumber;
        
        GetInputInt("Action:", &selection);
        switch (selection) {
            case 1:
                printf("Input your card number: ");
                GetInputInt("", &cardNumber);
                addCard(state, cardNumber);
                break;
            case 2:
                printf("\n");
                return;
            default:
                printf("Invalid selection. Please try again.\n");
                break;
        }
    }
}