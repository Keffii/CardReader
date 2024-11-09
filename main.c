#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "main.h"
#include "SafeInput.h"

void saveCardsToFile(const SystemState* state) {
    FILE* file = fopen("cards.txt", "w");
    if (file == NULL) {
        printf("Error: Unable to open file for writing.\n");
        return;
    }

    for (int i = 0; i < state->cardCount; i++) {
        fprintf(file, "Date: %s, Card: %d, Access: %s\n",
                state->cards[i].dateAdded,
                state->cards[i].cardNumber,
                state->cards[i].hasAccess ? "Yes" : "No");
    }

    fclose(file);
}


void loadCardsFromFile(SystemState* state) {
    FILE* file = fopen("cards.txt", "r");
    if (file == NULL) {
        // It's okay if the file doesn't exist yet
        state->cardCount = 0;
        state->cards = NULL;
        return;
    }

    // Initialize card count
    state->cardCount = 0;
    state->cards = NULL;

    // Variables for parsing
    char line[256];
    int cardNumber;
    char accessStr[4];
    char dateStr[20];

    // Read each line
    while (fgets(line, sizeof(line), file)) {
        // Parse the line according to the format: "Date: %s, Card: %d, Access: %s"
        if (sscanf(line, "Date: %[^,], Card: %d, Access: %s\n",
                   dateStr, &cardNumber, accessStr) == 3) {
            
            // Reallocate memory for the new card
            Card* temp = realloc(state->cards, (state->cardCount + 1) * sizeof(Card));
            if (temp == NULL) {
                printf("Error: Memory allocation failed.\n");
                fclose(file);
                return;
            }
            state->cards = temp;

            // Fill in the card data
            state->cards[state->cardCount].cardNumber = cardNumber;
            state->cards[state->cardCount].hasAccess = (strcmp(accessStr, "Yes") == 0);
            strncpy(state->cards[state->cardCount].dateAdded, 
                    dateStr, 
                    sizeof(state->cards[state->cardCount].dateAdded) - 1);
            state->cards[state->cardCount].dateAdded[sizeof(state->cards[state->cardCount].dateAdded) - 1] = '\0';
            
            state->cardCount++;
        }
    }

    fclose(file);
    printf("Loaded %d cards from file.\n", state->cardCount);
}


void remoteOpenDoor(SystemState* state){
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


void listAllCards(SystemState* state) {
    printf("*** All cards in the system ***\n\n");
    bool cardFound = false;
    for (int i = 0; i < state->cardCount; i++) {
        printf("*** Info about card ***\n");
        printf("Card Number: %d\n", state->cards[i].cardNumber);
        printf("Has Access: %s\n", state->cards[i].hasAccess ? "Yes" : "No");
        printf("Date Added: %s\n", state->cards[i].dateAdded);
        printf("\n");
        cardFound = true;
    }
    if(!cardFound){
        printf("No cards found\n");
    }
}


//Add/Remove Access
void addCard(SystemState* state, int cardNumber) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    
    // Check if the card already exists
    bool cardExists = false;
    for (int i = 0; i < state->cardCount; i++) {
        if (state->cards[i].cardNumber == cardNumber) {
            cardExists = true;
            if (state->cards[i].hasAccess) {
                // If card has access, remove access
                state->cards[i].hasAccess = false;
                printf("Access removed from card number: %d\n", cardNumber);
            } else {
                // If card already has no access, toggle it to have access
                state->cards[i].hasAccess = true;
                printf("Access granted to card number: %d\n", cardNumber);
            }
            break;
        }
    }

    if (!cardExists) {
        // Allocate space for the new card
        Card* temp = realloc(state->cards, (state->cardCount + 1) * sizeof(Card));
        if (temp == NULL) {
            printf("Error: Unable to allocate memory for new card.\n");
            return;
        }
        state->cards = temp;
        
        // Add the new card
        sprintf(state->cards[state->cardCount].dateAdded, "%04d-%02d-%02d-%02d:%02d", 
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min);
        state->cards[state->cardCount].cardNumber = cardNumber;
        state->cards[state->cardCount].hasAccess = true;
        state->cardCount++;
        printf("New card %d added successfully with date %s.\n", 
               cardNumber, state->cards[state->cardCount - 1].dateAdded);
    }

    // Save cards to file after any changes
    saveCardsToFile(state);
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

void fakeCardScan(SystemState* state) {
    int scannedCardNumber;
    printf("\n*** Card Scanner Simulation ***\n");
    printf("Please scan your card (enter card number): ");
    GetInputInt("", &scannedCardNumber);

    bool cardFound = false;
    for (int i = 0; i < state->cardCount; i++) {
        if (state->cards[i].cardNumber == scannedCardNumber) {
            cardFound = true;        
            if (state->cards[i].hasAccess) {
                printf("Card %d has access\n", scannedCardNumber);
            }
            else {
                state->cards[i].hasAccess = false;
                printf("Card %d has no access\n", scannedCardNumber);
            }
            break;
        }
    }
    if (!cardFound) {
        printf("Card not found\n");
    }
}


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