#include<stdio.h>
 

#define MIN_GRID_SIZE 5
#define MAX_GRID_SIZE 15
#define MAX_INTEL_COLLECTABLE 3
#define MAX_LIVES_COLLECTABLE 2
#define Player_Symbol '@'
#define Wall '#'
#define Intel 'I'
#define Lives 'L'
#define Extraction_Point 'X'
#define Empty_Space '.'
#define LOG_FILE "spynet_game_state.txt"

typedef struct {
          int x; // Position in the row of the grid
          int y; // Position in the column of the grid
} Position;

typedef struct {
          int total_lives;// Player's Initial lives + Player's collected lives during the game is stored here
          int intel_collected; // The number of intels collected by Player during the game is stored here
          int is_active ; // 1 is stored here for "Active" status of player while o is stored here for "Inactive" status of player
          Position position; // stores the position of the payer in the grid in terms of x(rows) and y(columns)
          int moves_count; // tracks the number of moves the player makes,avoid s the need to declare this variable in the main program
} Player;
typedef struct {
          char ** grid; // this is the array of characters that represents the game grid
          int N; // stores the grid size : N x N
          Position extraction; // stores the x and y coordinate of the extraction point
          int Intel_Remaining ; // stores the number of intel remaining,this reduces the need to check the grid each and every time for the count of intel
          int Lives_Remaining ; // stores the number of Lives remaining,this reduced the need to check the grid each and every time for the count of lives
} Grid ;

typedef struct {
          Player player; // tracks the player
          Grid *grid; // a pointer to the game grid
          int game_terminated ; // this is flag that signals the end of the game
          int game_won ; // this stores two values : '1' for victory and '0' for loss.
          
} GameState;

/* declaration of all the functions used in this program are given below*/
Grid * create_grid(int n); //a function that creates a grid and return a memory address of the grid
void clear_grid(Grid* grid); // frees all memory to prevent memory leakage
void place_items_randomly(Grid *grid,Position *player_start);// sets the intel.lives,walls,extraction and the starting co-ordinates of the player randomly
void generate_walls(Grid* grid);// a function to generate the walls in the grid
void display_grid(Grid *grid); // a function to display the grid
void initialise_game(GameState *game,int grid_size);// starts the game
void display_player_status(Player *player); // displays the player's status every time before a player makes his move
void display_game_instructions(void);// displays the instructions of the game to the player
char get_movement_key(void); // a function that returns the user's input of movement
int validate_move(GameState* game, char direction);
void handle_invalid_move(GameState *game); // penalises for invalid input by reducing player life and displaying warning message
void collect_item(GameState *game,int x,int y); // updates player status accordingly whenever player collects intel or lives,removes the collected item from the grid and displays the item collected.
void check_win_conditions(GameState *game);// checks whether the player has met the requirement to win the game.
int initialise_log_file(void); // creates the log file to store game state
void log_game_state(GameState *game,char* Action_Description);// saves the game state
void update_player_state(GameState *game,char direction);// after each move,the position,lives and intel count and move is validated.
int random_number_generator(int min,int max);// a function that generate a random number within a specified range
int  get_grid_size(void); // Gets the grid size from the user
void show_game_results(GameState *game); // indicates to the player whether they won or lost the game.
void clear_grid(Grid *grid);
int main(){
         srand(time(NULL)); // This function ensures each game produces random numbers for the placement of items in the grid.
        if(initialise_log_file() == 0) { // creates the log file to store game status after each move by the user.
                                     printf("Error creating log file!Exiting program!Try again!");
                                     return 1;
         }                                    
         int N = get_grid_size(); // gets the grid size from the user and stores it into variable 'N'
         
         GameState game; // declare game as the game state structure.
         if (initialise_game(&game, N) == 0) {  // checks if the grid was successfully created.
                                       printf("Failed to initialize game! Exiting.\n"); // if unsuccessful,it outputs an error message and exits the program.
                                       return 1;
         }
         display_game_instructions(); // function to instruct the player about the rules and regulations of the game.
         while (game.game_terminated == 0){  // checks if the game is still ongoing.
                              display_grid(game.grid); // displays the grid to the player
                              display_player_status(&game.player); // displays the current status of the player
                              char move = get_movement_key(); // gets the movement key inputted by the user and stores it in variable ' move'
                              move = toupper(move); // converts the movement key input to uppercase in case user prefers to enter lowercase letters
                              if(move == 'Q') { // checks if the users prefers to quit the game
                                              printf("You have terminated the game!\n"); // displays an output of acknowledgement and subsequently exits the game.
                                              game.game_won = 0; // indicates that the player did not win current game
                                              game.game_terminated = 1; // indicates that game is terminated
                                              game.player.is_active = 0; // indicates that player is currently inactive.
                                              log_game_state(&game,"Player Quit!");
                                              break; // used to exit the game.
                              }
                              if(validate_move(&game,move) == 1){ // checks if the move chosen by player is valid
                                                     update_player_state(&game,move); // updates the status of the player accordingly.
                                                     log_game_state(&game,"Valid Move!"); // log the state of the game after each player.
                                                     game.player.moves_count++; // increment the count of the player
                               } else {
                                         handle_invalid_move(&game); // calls a function to handle invalid movement within the game.
                                         log_game_state(&game,"Invalid Move!"); // logs the status of the movement of player as "invalid Move!
                               }
                               check_win_conditions(&game); // checks if the player has met the requirements to win the game.
        }
        show_game_results(&game); // the game is terminated once the player has decided to quit the game,has lost the game or won the game.Afterwards this function is called to show the results of the game to the player.
         clear_grid(game.grid); // frees dynamically allocated memory to prevent memory leaks.
        return 0;
}

void display_game_instructions() {
   /* This function displays the instructions that needs to followed during the game*/
    printf("Welcome to the game: SpyNet - The Codebreaker Protocol!\n");
    printf("Please read and understand the instructions to this game carefully.\n");
    printf("The information will be displayed below shortly...\n\n");
    
    printf("Collect all three Intel Items shown in the game grid and reach the extraction point.\n\n");
    
    printf("If you collide with a wall, step out of the grid, or enter an invalid direction: \n");
    printf("your number of lives will decrease by 1!\n\n");
    
    printf("You have 3 lives at the start of the game and 2 extra lives randomly placed in the grid which you can collect to increase your number of lives!\n\n");
    
    printf("If you reach the extraction point without collecting all the three Intels in the grid,\n");
    printf("you lose the game. Additionally, if you decide to quit the game or run out of lives...\n");
    printf("you automatically lose the game!\n\n");
    
    printf("These are the movement keys that you need to type to navigate through the grid:\n");
    printf("  'W' to move up by one cell.\n");
    printf("  'A' to move left by one cell.\n");
    printf("  'S' to move down by one cell.\n");
    printf("  'D' to move right by one cell.\n");
    printf("  'Q' to quit the game.\n\n");
    
    printf("Please take note of the game symbols that will be used to represent the game grid:\n");
    printf("  '%c' - Your position in the grid!\n", Player_Symbol);
    printf("  '%c' - Wall (You need to avoid these!)\n", Wall);
    printf("  '%c' - Intel (collect all 3 of these in the grid to win)\n", Intel);
    printf("  '%c' - Extra Life (collect to increase lives)\n", Lives);
    printf("  '%c' - Extraction Point (reach after collecting all Intel)\n", Extraction_Point);
    printf("  '%c' - Empty space (safe to move through)\n\n", Empty_Space);
    
    printf("Ok, those are all the instructions!\n");
    printf("I wish you good luck winning this game, player!\n");
    printf("--Get ready to show your skills!\n");
}
char get_movement_key(){ 
                       char move_key; // declaration of variable to store user's choice of movement.
                       printf("Enter your move! (W/A/S/D/Q)\n"); // displays the options that can be selected to the user
                       scanf("%c",&move_key); // inputs the user's choice
                       return move_key; // returns the user's input.
}
int validate_move(GameState *game,char direction){
                       int new_x = game-> player.position.x; // stores the current row of where the Player is in the game grid.
                       int new_y = game-> player.position.y; // stores the current column of where the Player is in the game grid.
                       switch(direction){
                                        case 'W' :
                                                 new_x--; // alters the row so that player can move upwards.
                                                 break;  
                                        case 'A' :
                                                 new_y--; // alters the column so that player can move left.
                                                 break;
                                        case 'S' :
                                                 new_x++; // alters the row so that player can move downwards.
                                                 break;
                                        case 'D' :
                                                 new_y++; // alters the column so that player can move right.
                                                 break;
                                        default :
                                                return 0;
                      }
                      if(new_x < 0 || new_x >= game->grid->N || new_y < 0 || new_y >= game->grid->N) // checks if user is within the grid
                                                                         return 0;
                      if(game->grid->grid[new_x][new_y] == Wall) // check if user has collided with a wall;
                                                               return 0;
                      return 1;
}
void handle_invalid_move(GameState *game){
                      printf("Invalid Move!\n"); // outputs the message that user has made an invalid move.
                      printf("You lost a life because you made an invalid move!\n"); 
                      game->player.total_lives--; // decreases the total number of lives the player has in the game.
                      if(game->player.total_lives<= 0){ // check if the player has no live remaining
                                                      game->player.total_lives = 0; 
                                                      game->player.is_active = 0; // updates the value in player structure to indicate that the player is not active.
                                                      printf("You have run out of lives!\n"); // outputs that the player has run out of lives.
                      } else
                            printf("You have %d lives remaining!\n",game->player.total_lives); // outputs the number of lives remaining if the player has lives.
}
void collect_item(GameState *game,int x,int y){
                      char Item = game ->grid ->grid[x][y]; // stores the Item available at the player's position in the game grid.
                      switch(Item) { // checks which Item it is : Intel or Life.
                                   case Intel :
                                              game->grid->Intel_Remaining--; // if Intel then the number of Intels to be displayed in the game grid decreases by 1.
                                              game->player.intel_collected++; // if Intel then the number of Intels collected by the player increases by 1/
                                              printf("You have collected an Intel! The total of collected intel is :%d \n",game->player.intel_collected); // notifies the player that they have successfully collected an intel.
                                              break;
                                   case Lives :
                                              game->grid->Lives_Remaining--; // if Life then the number of Lives to be displayed in the game grid decreases by 1
                                              game->player.total_lives++; // if Life then the total number of Lives collected by the Player increases by 1.
                                              printf("You have collected a live! You have a total of : %d\n",game-> player.total_lives); // notifies the player the total number of lives they have.
                                              break;
                                   }
                                   if (Item == Intel || Item == Lives) 
                                                                   game->grid->grid[x][y] = Empty_Space; // removes the item after it is collected by the player.
}
int initialise_log_file(){
                      FILE *fptr = fopen(LOG_FILE,"w"); // creates a file to log the game state.
                      if(fptr == NULL) // check if the file was created successfully.
                                    return 0; // if unsuccessful returns 0 to the main program.
                      fprintf(fptr, "=== SPYNET GAME LOG ===\n");
                      fclose(fptr)                      
                      return 1; 
}

int random_number_generator(int min, int max) {
    return min + rand() % (max - min + 1); // this is the formula for generating random numbers
}

void display_player_status(Player *player){
    /* This function displays the player's status!*/
    printf("Your status is : \n");
    printf("You are currently at (%d,%d) on the game grid.", player->position.x, player->position.y);
    printf("You have %d lives remaining!",player ->total_lives);
    printf("You have so far collected %d Intels out of %d Intel!", player -> intel_collected,MAX_INTEL_COLLECTABLE);
    printf("You have made a total of %d moves till now...", player -> moves_count);
}
void clear_grid(Grid* grid) {
    if (!grid) return; //  check if the grid pointer is null because if the function tries to clear a null pointer the function will immediately exit.
    
    if (grid->grid) { // check if the pointer to the array is not null.
        for (int i = 0; i < grid->N; i++) {
            free(grid->grid[i]); // frees each element in the array.row by row. 
        }
        free(grid->grid); // frees the memory that was allocated to each row.
    }
    free(grid); // frees the memory allocated to the row of pointers.
}
