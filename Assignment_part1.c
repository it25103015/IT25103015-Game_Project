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
          Position ; // stores the x and y coordinate of the extraction point
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
void initialise_log_file(void); // creates the log file to store game state
void log_game_state(GameState *game,char* Action_Description);// saves the game state
void update_player_state(GameState *game,char direction);// after each move,the position,lives and intel count and move is validated.
int random_number_generator(int min,int max);// a function that generate a random number within a specified range
int  get_grid_size(void); // Gets the grid size from the user
void show_game_results(GameState *game); // indicates to the player whether they won or lost the game.
void clear_grid(Grid *grid);
int main(){
         srand(time(NULL));
         initialise_log_file();
         int N = get_grid_size();
         
         GameState game;
         if (initialise_game(&game, N) == 0) {  // Should return success/failure
                                       printf("Failed to initialize game! Exiting.\n");
                                       return 1;
         }
         display_game_instructions();
         while (game.game_terminated == 0){
                              display_grid(game.grid);
                              display_player_status(&game.player);
                              char move = get_movement_key();
                              move = toupper(move);
                              if(move == 'Q') {
                                              printf("You have terminated the game!\n");
                                              game.game_won = 0;
                                              game.game_terminated = 1;
                                              game.player.is_active = 0;
                                              break;
                              }
                              if(validate_move(&game,move) == 1){
                                                     update_player_state(&game,move);
                                                     log_game_state(&game,"Valid Move!");
                                                     game.player.moves_count++;
                               } else {
                                         handle_invalid_move(&game);
                                         log_game_state(&game,"Invalid Move!");
                               }
                               check_win_conditions(&game);
        }
        show_game_results(&game);
         clear_grid(game.grid);
        return 0;
}


