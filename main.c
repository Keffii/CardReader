#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "main.h"
#include "SafeInput.h"


//Remote open door
void opendoor(SystemState* state){
    int cardNumberToUse;
    printf("Enter the card number to use for opening the door: ");
    GetInputInt("", &cardNumberToUse);

    // Search for the card in the system
    bool cardFound = false;
    for (int i = 0; i < state->cardCount; i++) {
        if (state->cards[i].cardNumber == cardNumberToUse) {
            cardFound = true;
            if (state->cards[i].hasAccess && !state->isDoorOpen) {
                printf("CURRENTLY LAMP IS: Green\n");
                state->isDoorOpen = true;
                sleep(3);
                printf("CURRENTLY LAMP IS: Red\n\n");
                state->isDoorOpen = false;
            } else {
                state->cards[i].hasAccess = false;
                printf("Card has no access\n");
            }
            break;
        }
    }

    if (!cardFound) {
        printf("Card not found in the system.\n");
    }
}

//List all cards
void listAllCards(SystemState* state) {
    printf("*** All cards in the system ***\n");
    bool cardFound = false;
    for (int i = 0; i < state->cardCount; i++) {
        printf("*** Info about card ***\n");
        printf("Card Number: %d\n", state->cards[i].cardNumber);
        printf("Has Access: %s\n", state->cards[i].hasAccess ? "Yes" : "No");
        printf("Date Added: %s\n", state->cards[i].dateAdded);
        cardFound = true;
    }
    if(!cardFound){
        printf("No cards found\n");
    }
}




//Add/Remove Access
void addCard(SystemState* state, int cardNumber) {
    // Check if the card already exists
    for (int i = 0; i < state->cardCount; i++) {
        if (state->cards[i].cardNumber == cardNumber) {
            // If card exists, check if it has access
            if (state->cards[i].hasAccess) {
                // If card has access, remove access
                state->cards[i].hasAccess = false;
                printf("Access removed from card number: %d\n", cardNumber);
            } else {
                // If card already has no access, toggle it to have access
                state->cards[i].hasAccess = true;
                printf("Access granted to card number: %d\n", cardNumber);
            }
            return;
        }
    }
    // Allocate space for the new card
    state->cards = realloc(state->cards, (state->cardCount + 1) * sizeof(Card));
    if (state->cards == NULL) {
        printf("Error: Unable to allocate memory for new card.\n");
        return;
    }


    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    // Format the date as YYYY-MM-DD
    sprintf(state->cards[state->cardCount].dateAdded, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

    // Add the new card
    state->cards[state->cardCount].cardNumber = cardNumber;
    state->cards[state->cardCount].hasAccess = true;
    state->cardCount++;

    printf("New card %d added successfully with date %s.\n", cardNumber, state->cards[state->cardCount - 1].dateAdded);
}


void removeAccess(SystemState* state, int cardNumber) {
    bool cardFound = false;
    for (int i = 0; i < state->cardCount; i++) {
        if (state->cards[i].cardNumber == cardNumber) {
            cardFound = true;
            state->cards[i].hasAccess = false;
            printf("Access removed for card %d.\n", cardNumber);
            break;
        }
    }

    if (!cardFound) {
        printf("Card %d not found in the system.", cardNumber);
    }

}

int main() {
    SystemState state = {NULL, 0, false}; // Initialize the SystemState struct

    while(true) {
        showMenu();
        int selection;
        GetInputInt("Action:", &selection);

        switch (selection) {
            case 1:
                printf("\n");
                opendoor(&state);
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
                // Fake card test scanning
                break;
            default:
                printf("Invalid selection. Please try again.\n");
                break;
        }
    }
}

void showMenu(){ 
    printf("\n");
    printf("Admin menu:\n\n");
    printf("1. Remote open door\n");
    printf("2. List all cards in system\n");
    printf("3. Add/Remove access\n");
    printf("4. Exit\n");
    printf("9. FAKE TEST SCAN CARD\n");
}

void addRemoveMenu(SystemState* state){
    while(true){
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



// void createCard(Card *p) {
//     printf("*** CREATE CARD ***\n");
//     printf("Card Number: ");
//     GetInputInt("", &p->cardNumber);
//     p->hasAccess = true; // or set to false if needed
//     printf("Date Added (YYYY-MM-DD): ");
//     GetInput("", p->dateAdded, sizeof(p->dateAdded) - 1);
// }

