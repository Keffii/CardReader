#ifndef CARD_SYSTEM_H
#define CARD_SYSTEM_H

#include <stdbool.h>

typedef struct {
    int cardNumber;
    char dateAdded[20];
    bool hasAccess;
} Card;

typedef struct {
    Card* cards;
    int cardCount;
    bool isDoorOpen;
} SystemState;

// File operations
void saveCardsToFile(const SystemState* state);
void loadCardsFromFile(SystemState* state);

// Card operations
void addCard(SystemState* state, int cardNumber);
void removeAccess(SystemState* state, int cardNumber);
void listAllCards(SystemState* state);
void remoteOpenDoor(SystemState* state);
void fakeCardScan(SystemState* state);

#endif