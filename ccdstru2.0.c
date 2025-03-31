#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Define constants
#define GRID_SIZE 4
#define MAX_POSITIONS 16

// Structure to represent a position
typedef struct {
    int x;
    int y;
} Position;

// Define sets as arrays of positions with size tracking
typedef struct {
    Position positions[MAX_POSITIONS];
    int size;
} PositionSet;

// Game state
typedef struct {
    PositionSet Uno;
    PositionSet Tres;
    PositionSet F;    // Free positions
    bool turn;
    bool go;
    bool over;
} GameState;

// Function prototypes
void initializeGame(GameState* game);
bool positionInSet(Position pos, PositionSet set);
void addPositionToSet(Position pos, PositionSet* set);
void removePositionFromSet(Position pos, PositionSet* set);
bool hasWinningPattern(PositionSet playerSet);
void checkGameOver(GameState* game);
bool processMove(GameState* game, Position pos);
void displayGame(GameState game);
void clearScreen();

// Winning patterns (as defined in set C)
const Position winningPatterns[4][4] = {
    {{1,1}, {1,2}, {1,3}, {1,4}},  // Top row
    {{1,1}, {2,2}, {3,3}, {4,4}},  // Main diagonal
    {{1,4}, {2,3}, {3,2}, {4,1}},  // Anti-diagonal
    {{4,1}, {4,2}, {4,3}, {4,4}}   // Right column
};

// Initialize a new game
void initializeGame(GameState* game) {
    // Clear sets
    game->Uno.size = 0;
    game->Tres.size = 0;
    
    // Initialize free positions (all positions are free initially)
    game->F.size = 0;
    for (int x = 1; x <= GRID_SIZE; x++) {
        for (int y = 1; y <= GRID_SIZE; y++) {
            game->F.positions[game->F.size].x = x;
            game->F.positions[game->F.size].y = y;
            game->F.size++;
        }
    }
    
    // Set initial state
    game->turn = true;
    game->go = false;
    game->over = false;
}

// Check if a position is in a set
bool positionInSet(Position pos, PositionSet set) {
    for (int i = 0; i < set.size; i++) {
        if (set.positions[i].x == pos.x && set.positions[i].y == pos.y) {
            return true;
        }
    }
    return false;
}

// Add a position to a set
void addPositionToSet(Position pos, PositionSet* set) {
    if (!positionInSet(pos, *set)) {
        set->positions[set->size] = pos;
        set->size++;
    }
}

// Remove a position from a set
void removePositionFromSet(Position pos, PositionSet* set) {
    for (int i = 0; i < set->size; i++) {
        if (set->positions[i].x == pos.x && set->positions[i].y == pos.y) {
            // Move the last position to this spot and decrease size
            set->positions[i] = set->positions[set->size - 1];
            set->size--;
            return;
        }
    }
}

// Check if a player has formed a winning pattern
bool hasWinningPattern(PositionSet playerSet) {
    // For each winning pattern
    for (int p = 0; p < 4; p++) {
        bool patternComplete = true;
        
        // Check if all positions in the pattern are in the player's set
        for (int i = 0; i < 4; i++) {
            if (!positionInSet(winningPatterns[p][i], playerSet)) {
                patternComplete = false;
                break;
            }
        }
        
        if (patternComplete) {
            return true;
        }
    }
    return false;
}

// Check if the game is over
void checkGameOver(GameState* game) {
    // Check winning conditions
    if (hasWinningPattern(game->Uno)) {
        game->over = true;
    }
    else if (hasWinningPattern(game->Tres)) {
        game->over = true;
    }
    else if (game->F.size == 0) {
        game->over = true;
    }
}

// Process a player move
bool processMove(GameState* game, Position pos) {
    // First case: Uno's turn (turn=true, go=true)
    if (game->turn && game->go && positionInSet(pos, game->F)) {
        // Add position to Uno's set
        addPositionToSet(pos, &game->Uno);
        // Remove from free positions
        removePositionFromSet(pos, &game->F);
        // Toggle turn and go
        game->turn = !game->turn;
        game->go = !game->go;
        return true;
    }
    // Second case: Removal turn (turn=false)
    else if (!game->turn) {
        // Check if position is in either Uno or Tres
        bool inUno = positionInSet(pos, game->Uno);
        bool inTres = positionInSet(pos, game->Tres);
        
        if (inUno || inTres) {
            // Remove position from respective set
            if (inUno) {
                removePositionFromSet(pos, &game->Uno);
            }
            if (inTres) {
                removePositionFromSet(pos, &game->Tres);
            }
            
            // Add back to free positions
            addPositionToSet(pos, &game->F);
            
            // Toggle turn
            game->turn = !game->turn;
            return true;
        }
    }
    // Third case: Tres's turn (turn=true, go=false)
    else if (game->turn && !game->go && positionInSet(pos, game->F)) {
        // Add position to Tres's set
        addPositionToSet(pos, &game->Tres);
        // Remove from free positions
        removePositionFromSet(pos, &game->F);
        // Toggle go
        game->go = !game->go;
        return true;
    }
    
    // Invalid move
    return false;
}

// Clear the screen (cross-platform)
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Display the current game state
void displayGame(GameState game) {
    clearScreen();
    
    printf("");
    printf("     GAME GRID\n\n");
    
    // Display coordinate reference above the board
    printf("    ");
    for (int x = 1; x <= GRID_SIZE; x++) {
        printf("%d   ", x);
    }
    printf("\n");
    
    // Display the board with simplified format
    for (int y = 1; y <= GRID_SIZE; y++) {
        printf("%d  ", y);  // Row coordinate
        
        for (int x = 1; x <= GRID_SIZE; x++) {
            Position currentPos = {x, y};
            if (positionInSet(currentPos, game.Uno)) {
                printf("\033[1;95m[U]\033[0m ");
            }
            else if (positionInSet(currentPos, game.Tres)) {
                printf("\033[1;94m[T]\033[0m ");
            }
            else {
                printf("[ ] ");
            }
        }

        printf("\n\n");
    }
    
    // Display game status
    printf("\nGame Status: ");
    if (game.over) {
        if (hasWinningPattern(game.Uno)) {
            printf("Game Over - Uno Wins!\n");
        }
        else if (hasWinningPattern(game.Tres)) {
            printf("Game Over - Tres Wins!\n");
        }
        else if (game.F.size == 0) {
            printf("Game Over - Dos Wins!\n");
        }
    } else {
        if (game.turn && game.go) {
            printf("\033[1;95mUno's Turn (Place a piece)\033[0m\n");
        }
        else if (game.turn && !game.go) {
            printf("\033[1;94mTres's Turn (Place a piece)\033[0m\n");
        }
        else {
            printf("\033[1;91mDos' Turn (Remove a U or T piece)\033[0m\n");
        }
    }
    
    // Display available moves
    if (!game.over) {
        if (!game.turn) {
            // Removal turn - show positions that can be removed
            printf("\nRemovable positions: ");
            bool foundPositions = false;
            
            for (int y = 1; y <= GRID_SIZE; y++) {
                for (int x = 1; x <= GRID_SIZE; x++) {
                    Position pos = {x, y};
                    if (positionInSet(pos, game.Uno) || positionInSet(pos, game.Tres)) {
                        printf("[%d,%d] ", x, y);
                        foundPositions = true;
                    }
                }
            }
            
            if (!foundPositions) {
                printf("None");
            }
            printf("\n");
        } else {
            // Placement turn - show free positions
            printf("\nAvailable positions: \n");
            for (int i = 0; i < game.F.size; i++) {
                printf("[%d,%d] ", game.F.positions[i].x, game.F.positions[i].y);
                if ((i + 1) % 8 == 0 && i < game.F.size - 1) {
                    printf("\n"); // Align continued list
                }
            }
            printf("\n\n");
        }
    }
    
}

int main() {
    GameState game;
    int x, y;
    Position movePos;
    
    // Initialize the game
    initializeGame(&game);
    
    // Game loop
    while (!game.over) {
        // Display current state
        displayGame(game);
        
        // Prompt for move
        printf("Enter coordinates (x y): ");
        if (scanf("%d %d", &x, &y) != 2) {
            // Clear input buffer if invalid input
            while (getchar() != '\n');
            printf("\n\\033[1;91mInvalid input! Please enter coordinates as two numbers (e.g., 1 2).\033[0m\n");
            printf("Press Enter to continue...");
            getchar();
            continue;
        }
        
        // Validate coordinate ranges
        if (x < 1 || x > GRID_SIZE || y < 1 || y > GRID_SIZE) {
            printf("\n\033[1;91mInvalid position! Coordinates must be between 1 and %d.\033[0m\n", GRID_SIZE);
            printf("Press Enter to continue...");
            getchar(); // Clear the newline
            getchar(); // Wait for Enter
            continue;
        }
        
        movePos.x = x;
        movePos.y = y;
        
        // Process the move
        if (!processMove(&game, movePos)) {
            printf("\nInvalid move! Try again.\n");
            printf("Press Enter to continue...");
            getchar(); // Clear the newline
            getchar(); // Wait for Enter
            continue;
        }
        
        // Check if game is over after the move
        checkGameOver(&game);
    }
    
    // Show final state
    displayGame(game);
    
    printf("Game Over! Press Enter to exit...");
    getchar(); // Clear the newline
    getchar(); // Wait for Enter
    
    return 0;
}