#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <thread>
#include <chrono>
#include <atomic>

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
    Card hand[CARDS_PER_PLAYER + 10]; //TODO: Player's cards (extra space for added cards) 
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
            {
                //INFO: Add special cards 
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
    }
     else if (card.value == -1) 
    {
        cout << card.color << " Skip";
    }
     else if (card.value == -2) 
    {
        cout << card.color << " Reverse";
    }
     else if (card.value == -3) 
    {
        cout << card.color << " Draw Two";
    }
     else if (card.value == -4) 
    {
        cout << "Wild";
    }
     else if (card.value == -5) 
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

//INFO: Draw a card for a player
void drawCard(Player &player, Card deck[], int &currentIndex)
{
    player.hand[player.handCount++] = deck[currentIndex++];
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
    atomic<bool> timeout(false);
    atomic<bool> gameOver(false);

    // Game timer thread
    thread gameTimer([&]() {
        this_thread::sleep_for(chrono::minutes(5));
        if (!gameOver) {
            cout << "\nGame over! No winner after 5 minutes.\n";
            exit(0);
        }
    });

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

        timeout = false;
        int choice = -1;
        atomic<bool> inputReceived(false);

        // Timer thread
        thread timerThread([&]() {
            this_thread::sleep_for(chrono::seconds(10));
            if (!inputReceived) {
                timeout = true;
                cout << "Time's over!\n";
            }
        });

        // Input thread
        thread inputThread([&]() {
            cout << "Choose a card to play (1-" << players[currentPlayer].handCount << "): ";
            cin >> choice;
            inputReceived = true;
        });

        timerThread.join();
        inputThread.join(); // Ensure both threads finish before proceeding.

        if (timeout) {
            drawCard(players[currentPlayer], deck, currentIndex);
            currentPlayer = (currentPlayer + 1) % 4;
            continue;
        }

        if (choice < 1 || choice > players[currentPlayer].handCount || 
            !canPlay(players[currentPlayer].hand[choice - 1], topCard)) {
            cout << "Invalid choice! You draw a card.\n";
            drawCard(players[currentPlayer], deck, currentIndex);
            currentPlayer = (currentPlayer + 1) % 4;
            continue;
        }

        // Process valid choice
        choice--;
        topCard = players[currentPlayer].hand[choice];

        // Remove the played card
        for (int i = choice; i < players[currentPlayer].handCount - 1; i++) {
            players[currentPlayer].hand[i] = players[currentPlayer].hand[i + 1];
        }
        players[currentPlayer].handCount--;

        // Check if the player has won
        if (players[currentPlayer].handCount == 0) {
            gameOver = true;
            cout << players[currentPlayer].name << " wins the game!\n";
            break;
        }

        currentPlayer = (currentPlayer + 1) % 4;
    }

    gameTimer.join(); // Ensure game timer thread finishes before exiting.
    return 0;
}
