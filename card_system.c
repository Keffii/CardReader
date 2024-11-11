#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "card_system.h"
#include "safeinput.h"

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
    // Open the file for reading
    FILE* file = fopen("cards.txt", "r");
    
    // If file doesn't exist, initialize empty state
    if (file == NULL) {
        state->cardCount = 0;
        state->cards = NULL;
        return;
    }

    // Start with empty state
    state->cardCount = 0;
    state->cards = NULL;

    // Buffer to store each line we read
    char line[256];
    
    // Variables to store the parts we read from each line
    int cardNumber;
    char dateStr[20];
    char accessStr[4];

    // Read the file line by line
    while (fgets(line, sizeof(line), file)) {
        // Try to read the values from the line
        // sscanf breaks down the line into parts based on the format
        // %[^,] means "read until you see a comma"
        // %d means "read a number"
        // %s means "read a string"
        int success = sscanf(line, "Date: %[^,], Card: %d, Access: %s\n",
                           dateStr, &cardNumber, accessStr);
        
        // If we successfully read all 3 parts
        if (success == 3) {
            // Make space for one more card
            int newSize = (state->cardCount + 1) * sizeof(Card);
            Card* temp = realloc(state->cards, newSize);
            
            // Check if we got the memory we asked for
            if (temp == NULL) {
                printf("Error: Couldn't allocate memory for new card.\n");
                fclose(file);
                return;
            }
            
            // Save the new memory location
            state->cards = temp;
            
            // Add the new card's info
            state->cards[state->cardCount].cardNumber = cardNumber;
            
            // Copy the date string
            int i;
            for (i = 0; dateStr[i] != '\0' && i < 19; i++) {
                state->cards[state->cardCount].dateAdded[i] = dateStr[i];
            }
            state->cards[state->cardCount].dateAdded[i] = '\0';
            
            // Set access (if accessStr is "Yes", set true, otherwise false)
            if (strcmp(accessStr, "Yes") == 0) {
                state->cards[state->cardCount].hasAccess = true;
            } else {
                state->cards[state->cardCount].hasAccess = false;
            }
            
            // Increase our count of cards
            state->cardCount++;
        }
    }

    // Close the file and print how many cards we loaded
    fclose(file);
    printf("Loaded %d cards from file.\n", state->cardCount);
}

void remoteOpenDoor(SystemState* state) {
    int cardNumberToUse;
    printf("Enter the card number to use for opening the door: ");
    GetInputInt("", &cardNumberToUse);

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

void addCard(SystemState* state, int cardNumber) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    
    bool cardExists = false;
    for (int i = 0; i < state->cardCount; i++) {
        if (state->cards[i].cardNumber == cardNumber) {
            cardExists = true;
            state->cards[i].hasAccess = !state->cards[i].hasAccess;
            printf("%s card number: %d\n", 
                   state->cards[i].hasAccess ? "Access granted to" : "Access removed from",
                   cardNumber);
            break;
        }
    }

    if (!cardExists) {
        Card* temp = realloc(state->cards, (state->cardCount + 1) * sizeof(Card));
        if (temp == NULL) {
            printf("Error: Unable to allocate memory for new card.\n");
            return;
        }
        state->cards = temp;
        
        sprintf(state->cards[state->cardCount].dateAdded, "%04d-%02d-%02d-%02d:%02d", 
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min);
        state->cards[state->cardCount].cardNumber = cardNumber;
        state->cards[state->cardCount].hasAccess = true;
        state->cardCount++;
        printf("New card %d added successfully with date %s.\n", 
               cardNumber, state->cards[state->cardCount - 1].dateAdded);
    }

    saveCardsToFile(state);
}

void listAllCards(SystemState* state) {
    printf("*** All cards in the system ***\n\n");
    if (state->cardCount == 0) {
        printf("No cards found\n");
        return;
    }

    for (int i = 0; i < state->cardCount; i++) {
        printf("*** Info about card ***\n");
        printf("Card Number: %d\n", state->cards[i].cardNumber);
        printf("Has Access: %s\n", state->cards[i].hasAccess ? "Yes" : "No");
        printf("Date Added: %s\n\n", state->cards[i].dateAdded);
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
            printf("Card %d has %saccess\n", 
                   scannedCardNumber, 
                   state->cards[i].hasAccess ? "" : "no ");
            break;
        }
    }
    if (!cardFound) {
        printf("Card not found\n");
    }
}