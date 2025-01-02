#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>

using namespace std;

#define TOTAL_CARDS 108
#define CARDS_PER_PLAYER 7

struct Card 
{
    char color[10];  //INFO: Card color Red, Green, Blue, Yellow, Wild 

    int value;       //INFO: Card value -1: Skip, -2: Reverse, -3: Draw Two, -4: Wild, -5: Wild Draw Four 
};

struct Player 
{
    char name[50];     //INFO: Player's name 
    Card hand[CARDS_PER_PLAYER]; //TODO: Player's cards 
    int handCount;     //INFO: Number of cards in the player's hand 
};

//INFO: Initialize the deck 

void initializeDeck(Card deck[]) 
{
    const char *colors[] = {"Red", "Green", "Blue", "Yellow"};
    int index = 0;

    //INFO: Add normal cards 0-9 and special cards Skip, Reverse, Draw Two 

    for (int i = 0; i < 4; i++) {
        for (int num = 0; num <= 9; num++) 
        {
            strcpy(deck[index].color, colors[i]); //INFO: Copy color 
            deck[index].value = num;
            index++;

            if (num != 0) 
            { //INFO: Add special cards 
                strcpy(deck[index].color, colors[i]);
                deck[index].value = -1; //INFO: Skip 
                index++;

                strcpy(deck[index].color, colors[i]);
                deck[index].value = -2; //INFO: Reverse 
                index++;

                strcpy(deck[index].color, colors[i]);
                deck[index].value = -3; //INFO: Draw Two 
                index++;
            }
        }
    }

    //TODO: Add Wild and Wild Draw Four cards 

    for (int i = 0; i < 4; i++) 
    {
        strcpy(deck[index].color, "Wild");
        deck[index].value = -4; //INFO: Wild 
        index++;

        strcpy(deck[index].color, "Wild");
        deck[index].value = -5; //INFO: Wild Draw Four 
        index++;
    } 
}


//TODO: Shuffle the deck 

void shuffleDeck(Card deck[]) 
{
    for (int i = 0; i < TOTAL_CARDS; i++) 
    {
        int j = rand() % TOTAL_CARDS;
        swap(deck[i], deck[j]);
    }
}

//INFO: Deal cards to players 

void dealCards(Player &player, Card deck[], int &currentIndex) 
{
    for (int i = 0; i < CARDS_PER_PLAYER; i++) 
    {
        player.hand[i] = deck[currentIndex++];
    }
    player.handCount = CARDS_PER_PLAYER;
}

 //INFO: Print a card 

void printCard(const Card &card) 
{
    if (card.value >= 0) 
    {
        cout << card.color << " " << card.value;
    } else if (card.value == -1) 
    {
        cout << card.color << " Skip";
    } else if (card.value == -2) 
    {
        cout << card.color << " Reverse";
    } else if (card.value == -3) 
    {
        cout << card.color << " Draw Two";
    } else if (card.value == -4) 
    {
        cout << "Wild";
    } else if (card.value == -5) 
    {
        cout << "Wild Draw Four";
    }
    cout << endl;
}

//INFO: Check if a card can be played 

bool canPlay(const Card &card, const Card &topCard) 
{
    return (card.value == -4 || card.value == -5 || card.value == topCard.value || strcmp(card.color, topCard.color) == 0);
}

                                              //HACK: Main game 

int main() 
{
    srand(time(0));

    Card deck[TOTAL_CARDS];
    Player players[4];
    int currentIndex = 0;

    initializeDeck(deck);
    shuffleDeck(deck);

    //INFO: Get player names and deal cards 

    for (int i = 0; i < 4; i++) 
    {
        cout << "Enter player " << i + 1 << " name: ";
        cin >> players[i].name;
        dealCards(players[i], deck, currentIndex);
    }

    Card topCard = deck[currentIndex++];
    int currentPlayer = 0;

    while (true) 
    {
        cout << "\n" << players[currentPlayer].name << "'s turn\n";
        cout << "Top card: ";
        printCard(topCard);

        cout << "Your hand:\n";
        for (int i = 0; i < players[currentPlayer].handCount; i++) 
        {
            cout << i + 1 << ". ";
            printCard(players[currentPlayer].hand[i]);
        }

        int choice;
        cout << "Choose a card to play (1-" << players[currentPlayer].handCount << "): ";
        cin >> choice;
        choice--;

        if (choice < 0 || choice >= players[currentPlayer].handCount || !canPlay(players[currentPlayer].hand[choice], topCard)) 
        {
            cout << "Invalid choice! Try again.\n";
            continue;
        }

        topCard = players[currentPlayer].hand[choice];

        //INFO: Remove the played card from the player's hand 

        for (int i = choice; i < players[currentPlayer].handCount - 1; i++) 
        {
            players[currentPlayer].hand[i] = players[currentPlayer].hand[i + 1];
        }
        players[currentPlayer].handCount--;

        //INFO: Check if the player has won 

        if (players[currentPlayer].handCount == 0) 
        {
            cout << players[currentPlayer].name << " wins the game!\n";
            break;
        }

        currentPlayer = (currentPlayer + 1) % 4;
    }

    return 0;
}
