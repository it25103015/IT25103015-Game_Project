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
#define GAME_STATE_FILE "spynet_game_state.txt"

typedef struct {
          int x; // Position in the row of the grid
          int y; // Position in the column of the grid
} Position;

typedef struct {
          int total_lives;// Player's Initial lives + Player's collected lives during the game is stored here
          int intel_collected; // The number of intels collected by Player during the game is stored here
          int is_active ; // 1 is stored here for "Active" status of player while o is stored here for "Inactive" status of player
          Position position; // stores the position of the payer in the grid in terms of x(rows) and y(columns)
} Player;
typedef struct {
          char ** grid; // this is the array of characters that represents the game grid
          int N; // stores the grid size : N x N
          Position Extraction_X; // stores the x and y coordinate of the extraction point
          int Intel_Remaining ; // stores the number of intel remaining,this reduces the need to check the grid each and every time for the count of intel
          int Lives_Remaining ; // stores the number of Lives remaining,this reduced the need to check the grid each and every time for the count of lives
} Grid ;

typedef struct {
          Player player; // tracks the player
          Grid *grid; // a pointer to the game grid
          int Game_Terminated ; // this is flag that signals the end of the game
          int Game_Won ; // this stores two values : '1' for victory and '0' for loss.
          int moves_count; // tracks the number of moves the player makes,avoid s the need to declare this variable in the main program
} Game_State;

/* definition of functions are given below*/
Grid * create_grid(int n); //a function that creates a grid and return a memory address of the grid
void clear_grid(Grid* grid); // frees all memory to prevent memory leakage
void Place_Items_Randomly(Grid *grid,Position *player_start);// sets the intel.lives,walls,extraction and the starting co-ordinates of the player randomly
void generate_walls(Grid* grid);// a function to generate the walls in the grid
void display_grid(Grid *grid); // a function to display the grid
void initialise_game()(Game_State *game,int grid_size);// starts the game
void display_player_status(Player *player); // displays the player's status every time before a player makes his move
void display_game_instructions();// displays the instructions of the game to the player
char get_movement_key(); // a function that returns the user's input of movement
int valid_input(char move); // check whether the movement key is valid.It returns '1'for valid input and '0' for invalid input.
int is_move_possible(Grid*grid,Player *player,char direction);//checks if the chosen movement key is possible to execute in the Grid.It returns '1' if possible and '0' for not possible
int is_within_bounds( Grid *grid,int x,int y); //This function is called inside is_move_possible() to check whether the updated co-ordinates are within the size of the grid.
void move_player(Game_State*game,char direction); // moves the Player accordingly after validation and updates game display and player position
void handle_invalid_movement_key(Game_State *game); // penalises for invalid input by reducing player life and displaying warning message
void collect_item(Game_State *game,int x,int y); // updates player status accordingly whenever player collects intel or lives,removes the collected item from the grid and displays the item collected.
void check_win_conditions(Game_State *game);// checks whether the player has met the requirement to win the game.
void initialise_log_file(); // creates the log file to store game state
void log_game_state(Game_State*game,char* Player_Action_Description);// saves the game state
void update_player_status(Game_State *game,char direction);// after each move,the position,lives and intel count and move is validated.
int random_number_generator(int min,int max);// a function that generate a random number within a specified range


int main(){


         return 0;
}


