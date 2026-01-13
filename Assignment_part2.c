#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>


#define MIN_GRID_SIZE 5
#define MAX_GRID_SIZE 15
#define MAX_INTEL_COLLECTABLE 3
#define MAX_LIVES_COLLECTABLE 2
#define Player1_Symbol '@'
#define Player2_Symbol '&'
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
          char symbol ;  // stores the characters : '@' or '&'
          int is_computer; // a variable to store user's choice of playing with computer or another player.
} Player;
typedef struct {
          char ** grid; // this is the array of characters that represents the game grid
          int N; // stores the grid size : N x N
          Position extraction; // stores the x and y coordinate of the extraction point
          int Intel_Remaining ; // stores the number of intel remaining,this reduces the need to check the grid each and every time for the count of intel
          int Lives_Remaining ; // stores the number of Lives remaining,this reduced the need to check the grid each and every time for the count of lives
} Grid ;

typedef struct {
          Player players[2]; // tracks the player
          Grid *grid; // a pointer to the game grid
          int game_terminated ; // this is flag that signals the end of the game
          int winner ; // if player1 has won it stores '0' if player2 then '1',if no winner '-1'
          int current_player; // '0' for player1 and '1' for player2.
          int active_players; // keeps track of the number of active players in the game.

} GameState;

/* declaration of all the functions used in this program are given below*/
Grid * create_grid(int n); //a function that creates a grid and return a memory address of the grid
void clear_grid(Grid* grid); // frees all memory to prevent memory leakage
void place_items_randomly_2player(Grid *grid,Player players );// sets the intel.lives,walls,extraction and the staring position of the two players
void generate_walls(Grid* grid);// a function to generate the walls in the grid
void display_grid(Grid *grid); // a function to display the grid
int initialise_game_2player(GameState *game,int grid_size);// starts the game for the two players
void display_all_player_status(Player *player); // displays the players' status every time before a player makes his move
void display_game_instructions(void);// displays the instructions of the game to the players
char get_movement_key(void); // a function that returns the user's input of movement
int validate_move_2player(GameState* game, char direction);
void handle_invalid_move_2player(GameState *game); // penalises for invalid input by reducing player life and displaying warning message
void collect_item_2player(GameState *game,int x,int y); // updates player status accordingly whenever player collects intel or lives,removes the collected item from the grid and displays the item collected.
void check_win_conditions_2player(GameState *game);// checks whether the player has met the requirement to win the game.
int initialise_log_file(void); // creates the log file to store game state
void log_game_state_2player(GameState *game,char* Action_Description);// saves the game state
void update_player_state_2player(GameState *game,char direction);// after each move,the position,lives and intel count and move is validated.
int random_number_generator(int min,int max);// a function that generate a random number within a specified range
int  get_grid_size(void); // Gets the grid size from the user
void show_game_results_2player(GameState *game); // indicates to the player whether they won or lost the game.
void display_movement_instructions(); // reminds the user of the movement keys' and symbols' description.
int choose_game_mode(); // functions to allow the user to choose whether he likes to play with  computer or a laptop.
char get_computer_move(); // function that return the computer's move if the user has chosen to play with computer.
int main(){
         srand(time(NULL)); // This function ensures each game produces random numbers for the placement of items in the grid.
        if(initialise_log_file() == 0) { // creates the log file to store game status after each move by the user.
                                     printf("Error creating log file!Exiting program!Try again!");
                                     return 1;
         }
         display_game_instructions_2player(); // function to instruct the players about the rules and regulations of the game.
         int N = get_grid_size(); // gets the grid size from the user and stores it into variable 'N'
         
         int player2_type = choose_game_mode(); 
         
         GameState game; // declare game as the game state structure.
         if (initialise_game_2player(&game, N,player2_type) == 0) {  // checks if the grid was successfully created.
                                       printf("Failed to initialize game! Exiting.\n"); // if unsuccessful,it outputs an error message and exits the program.
                                       return 1;
         }
         printf("Game initialized successfully!\n");
         
         while (game.game_terminated == 0){  // checks if the game is still ongoing.
                              display_grid(game.grid); // displays the grid to the player
                              display_all_player_status(&game); // displays the current status of the player
                              Player *current = &game.players[game.current_player]; // puts the current player into an array of structures.
                              
                              if(current->is_active == 0) {
                                                  printf("Player %d with symbol %c is inactive!\nSkipping turn...\n",game.current_player + 1,current->symbol); // identifies with player is inactive and notifies that the his/her turn is being switched
                                                  game.current_player = (game.current_player + 1) % 2; // moves the number to next player 
                                                  continue ; // game continues without the player who is inactive.
                              }
                              printf("\n=== Player %d's turn  ===\n",game.current_player + 1); // notified which player's turn it is.
                              
                              char move; // a variable to store the players' moves.
                              if(current->is_computer)  
                                               move = get_computer_move();
                              else {
                                    display_movement_instructions(); // displays the movement instructions again as a reminder for the players.
                                    move = get_movement_key(); // gets the movement key inputted by the user and stores it in variable ' move'
                              }
                              move = toupper(move); // converts the movement key input to uppercase in case user prefers to enter lowercase letters
                              if(move == 'Q') { // checks if the users prefers to quit the game
                                              printf("Player %d has quit the game!\n",game.current_player + 1); // displays when a player decided to quit.
                                              current -> is_active = 0; // modifies the player's current state to inactive
                                              game.active_players--; // decreases the count of the total active players in the game.
                                              log_game_state_2player(&game,"Player Quit!"); // calls this function to log the game state.
                                              check_win_conditions_2player(&game);
                                              if (game.game_terminated == 0) {
                                                              game.current_player = (game.current_player + 1) % 2 ;
                                                          
                                              }
                                              continue;
                              }
                              if(validate_move_2player(&game,move) == 1){ // checks if the move chosen by player is valid
                                                     update_player_state_2player(&game,move); // updates the status of the player accordingly.
                                                     current->moves_count++; // tracks the current nove number by player.
                                                     log_game_state_2player(&game,"Valid Move!"); //log the state of the game after each player.
                                                     
                                                   
                               } else {
                                         handle_invalid_move_2player(&game); // calls a function to handle invalid movement within the game.
                                         log_game_state_2player(&game,"Invalid Move!"); // logs the status of the movement of player as "invalid Move!
                                         

                               }
                               check_win_conditions_2player(&game); // checks if the player still active has met the requirements to win the game.
                               if (game.game_terminated == 0) 
                                         game.current_player = (game.current_player + 1) % 2;  // switches to next player.
         }
         show_game_results_2player(&game); // the game is terminated once a player has decided to quit the game,has lost the game or has used up all his/her lives.Afterwards this function is called to show the results of the game to the players.

         clear_grid(game.grid); // frees dynamically allocated memory to prevent memory leaks.
        return 0;
}
void display_movement_instructions(){
    printf("These are the movement keys that you need to type to navigate through the grid:\n");
    printf("  'W' to move up by one cell.\n");
    printf("  'A' to move left by one cell.\n");
    printf("  'S' to move down by one cell.\n");
    printf("  'D' to move right by one cell.\n");
    printf("  'Q' to quit the game.\n\n");
    printf("  '%c' - Your position in the grid!\n", Player_Symbol);
    printf("  '%c' - Wall (You need to avoid these!)\n", Wall);
    printf("  '%c' - Intel (collect all 3 of these in the grid to win)\n", Intel);
    printf("  '%c' - Extra Life (collect to increase lives)\n", Lives);
    printf("  '%c' - Extraction Point (reach after collecting all Intel)\n", Extraction_Point);
    printf("  '%c' - Empty space (safe to move through)\n\n", Empty_Space);
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
                       scanf(" %c",&move_key); // inputs the user's choice
                       while (getchar() != '\n'); // clears any leftover characters and keeps the buffer empty.
                       return move_key; // returns the user's input.
}
int validate_move_2player(GameState *game,char direction){
                       Player *current = &game->players[game->current_player]; // the variable 'current' points to the memory address of the currect player
                       int new_x = current -> position.x; // stores the current row of where the Player is in the game grid.
                       int new_y = current -> position.y; // stores the current column of where the Player is in the game grid.
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
                      char target_cell = game->grid->grid[new_x][new_y]; // checks the cell that has been updated
                      if (target_cell == Player1_Symbol || target_cell == Player2_Symbol) {  // if the updated cell already has the symbol of Player1 or Player2 it cannot be placed.
                             
                               for (int i = 0; i < MAX_PLAYERS; i++) { // finds which player is at that position
                                          if (i != game->current_player && game->players[i].is_active && game->players[i].position.x == new_x && game->players[i].position.y == new_y) {
                                                                           return 0; // if another player is already in that position,the updated movements cannot be made.
                                          }
                               }
                      }
                      return 1;
}
void handle_invalid_move_2player(GameState *game){
                      Player *current = &game->players[game->current_player]; // the variable 'current' points to the memory address of the currect player/
                      printf("Invalid Move!\n"); // outputs the message that user has made an invalid move.
                      printf("Player %d lost a life because he/she made an invalid move!\n",game->current_player + 1);

                      current -> total_lives--; // decreases the total number of lives of that player by 1.
                     
                      if(current ->total_lives<= 0){ // check if the current player has no lives remaining
                                                      current -> total_lives = 0; // updates the current life of player to 0.
                                                      current -> is_active = 0; // updates the value in player structure to indicate that the player is not active.
                                                      game->active_players--; // decreases the total number of active players in the game by 1.
                                                      printf("Player %d has run out of lives!\n",game->current_player + 1); // outputs that the player who has run out of lives.
                      } else
                            printf("Player %d has %d lives remaining!\n",game->current_player + 1,current -> total_lives); // outputs the number of lives remaining if the player has lives.
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
                      fclose(fptr);
                      return 1;
}

int random_number_generator(int min, int max) {
    return min + rand() % (max - min + 1); // this is the formula for generating random numbers
}

void display_player_status(Player *player) {
    printf("\n=== PLAYER STATUS ===\n");
    printf("Position: (%d, %d)\n", player->position.x, player->position.y); // displays the current position of player in the game grid.
    printf("Lives: %d\n", player->total_lives); // displays the number of lives,the player has
    printf("Intel Collected: %d/%d, %d more to collect to win the game!\n", player->intel_collected, MAX_INTEL_COLLECTABLE,MAX_INTEL_COLLECTABLE - player->intel_collected); // displays the number of intels,the player has collected as a fraction.
    printf("Total Moves: %d\n", player->moves_count); // displays the total number of moves made by the player so far.
    printf("====================\n");
}
Grid * create_grid(int n) {
                 Grid *grid = (Grid*)malloc(sizeof(Grid)); // dynamically allocates memory for the Grid Structure
                 if(grid == NULL) // if memory allocation fails,'NULL' is returned by malloc()
                                return NULL; // NULL is returned to main memory because the Grid structure was not correctly allocated memory 

                 grid->N = n; // sets the grid size to n
                 grid->Intel_Remaining = MAX_INTEL_COLLECTABLE; // sets the Intel_Remaining member in grid structure to 3
                 grid->Lives_Remaining = MAX_LIVES_COLLECTABLE; // sets the Lives_Remaining member in grid structure to 2

                 grid->grid = (char**)malloc(n * sizeof(char*)); // creates an array of n pointers

                 if(grid ->grid == NULL){ // checks if the creating of an array fails
                                        free(grid); // frees the grid structure that was created earlier.
                                        return NULL; // return NULL to main program to indicate that the grid structure and array was not created properly
                 }

                 for(int i = 0;i < n;i++){
                             grid->grid[i] = (char*)malloc(n * sizeof(char)); // allocates a memory address for each row
                             if(grid->grid[i] == NULL) { // check if memory address allocation to a row  has failed.
                                             for(int j = 0;j < i; j++)
                                                            free(grid->grid[j]); // frees memory from  rows : 0 to i(the ones that were successfuly allocated memory)

                                             free(grid->grid); //free the array of pointers.
                                             free(grid); // frees the Grid structure
                                             return NULL; // return NULL becasue row allocation has caused the creation of Game Grid to stop.
                             }
                             for(int j = 0;j < n; j++) // if memory allocation of the Game Grid array succeed then this is executed.
                                           grid->grid[i][j] = Empty_Space; // initialises all cells in this row to Empty_Space symbol.
                }
                return grid;// returns the successfully created grid.
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

void log_game_state_2player(GameState *game,char* Action_Description) {
                                       FILE *fptr = fopen(LOG_FILE,"a"); // creates a file pointer that points to the memory address of the file being accessed.
                                       if(fptr == NULL){ // checks if the file pointer is valid
                                                     printf("Error opening file!\n"); // displays error message if the file pointer is invalid.
                                                     return;
                                       }
                                       int total_moves = game->players[0].moves_count + game->players[1].moves_count; // calcualtes the total moves
                                       fprintf(fptr,"\nMove Number : %d\n",total_moves); // writes the current move number of the the player,
                                      
                                       fprintf(fptr,"Action : %s\n",Action_Description); // write the current Action that was chosen by the Player
                                       for (int i = 0; i < MAX_PLAYERS; i++) {
                                                           fprintf(fptr,"Player %d's  position is : (%d , %d)\n",i + 1 ,game->players[i].position.x,game->players[i].position.y); // writes the current player's co-ordinates in the grid.
                                       fprintf(fptr,"Player has a total of %d lives\n",game->players[i].total_lives); // writes the total number of lives the player currently has.
                                       fprintf(fptr,"Intel Collected : %d/%d\n",game->players[i].intel_collected,MAX_INTEL_COLLECTABLE); // writes the number of intels collected as a fraction of the total.
                                       }

                                       for(int i = 0;i < game->grid->N;i++){
                                                          for(int j = 0;j < game->grid->N;j++)
                                                                         fprintf(fptr,"%c ",game->grid->grid[i][j]); // writes the state of the grid(location of life,intel etc...)
                                                          fprintf(fptr,"\n");
                                       }

                                       fclose(fptr);
}
void check_win_conditions_2player(GameState *game){
               
                Player *current = &game->players[game->current_player]; // a variable named 'current' that points to the current player

                int Is_Extraction_Point; // a variable to indicate whether player has reached the extraction point.'0' for no and '1' for yes.
                Is_Extraction_Point = (current -> position.x == game->grid->extraction.x) && (current -> position.y == game->grid->extraction.y); // if the current x and y co-ordinates of the player in the grid is the extraction point then '1' is put to the Is_Extraction_Point or '0' if not the extraction poitn.
                if(Is_Extraction_Point == 1){ // if the player has reached the extraction point
                                            if(current -> intel_collected == MAX_INTEL_COLLECTABLE){ // checks if the current player has collected all the intel to secure victory.
                                                                            game->game_terminated = 1; // updates the game's state to terminated.
                                                                            game-> winner = game -> current_player; // indicates that the current player has won the game.
                                                                            return;
                                            } else {
                                                    current ->is_active = 0; // indicates that the current player is no longer active.
                                                    game->active_players--; // decreases the total number of active players by 1 since the current player is no longer active.
                                                   }
               } else if (current -> total_lives <= 0 && current -> is_active){ // if the current player has run out of lives and is active
                                                        current -> is_active = 0; // updates the current player's state to inactive
                                                        game->active_players--; // decreases the total number of active players by 1 since the current player is no longer active.
                                                       }
                
                if(game -> active_players == 1) {
                                       game -> game_terminated = 1; // if only one player is active,automatically he/she becomes the victor.
                                       
                                       for (int i = 0; i < MAX_PLAYERS; i++) {
                                                      if (game->players[i].is_active) { // goes in a loop to find the active player
                                                                        game->winner = i;
                                                                        break; // exits the loop,once found.
                                                      }
                                       }
                
                } else if(game -> active_players == 0) { // if no active players are present
                                       
                                       game->game_terminated = 1; // indicates that the game is terminated.
                                       game -> winner = -1; // indicates that there was no winner in this round.
                 }
}


void display_grid(Grid *grid) {


    printf("\n=== GAME GRID ( %d x %d ) ===\n", grid->N, grid->N);

    // Column numbers
    printf("   ");
    for (int j = 0; j < grid->N; j++) {
        printf("%2d ", j); // displays the column numbers in the grid.
    }
    printf("\n");

    // Grid with row numbers
    for (int i = 0; i < grid->N; i++) {
        printf("%2d ", i); // displays the row numbers in the grid.
        for (int j = 0; j < grid->N; j++) {
            printf(" %c ", grid->grid[i][j]); // displays the cells of the grid game.
        }
        printf("\n");
    }
    printf("========================\n");
}
void show_game_results_2player(GameState *game){
               /* It shows the results of the game,whether the player has won or lost the game.*/
               printf("\n=== Game Over! ===\n");
               if(game-> winner == 0) {
                              printf("=== Player1 is the winner!=== \n Congratulations!\n");
                              printf("Your progress has been saved to %s\n.Please check that out and always be a victor!\n",LOG_FILE);
                  
                              if(game ->players[1].is_computer)
                                                   printf("You have defeated the computer!Keep it up!\n");
                              else 
                                                   printf("You have defeated Player2.keep it up!\n");
               }
               else if(game-> winner == 1) {
                              printf("=== Player2 is the winner!=== \n Congratulations!\n");
                              printf("Your progress has been saved to %s\n.Please check that out and always be a victor!\n",LOG_FILE);

                              if(game ->players[1].is_computer) {
                                                   printf("Your computer rival won this round!\n");
                                                   printf("Better luck next time!\n");
                              }
                              else
                                                   printf("You have defeated Player1.\nKeep it up!\n");
               }
               else {
                             printf("NO WINNER - MISSION FAILED\n");
                             printf("All players have been eliminated or have quitted!\n");
                             printf("The mission was unsuccessful,better luck next time!\n");
                             printf("Your progress has been saved to %s\nPlease check that out and plan a better strategy!\n",LOG_FILE);
               }
}

void generate_walls(Grid* grid){
               int total_cells = grid->N * grid->N; // a variable to to store the total number of cells in the grid : N x N
               int total_wall_number = total_cells/5; // the percentage of walls occupied in the grid is 20 percentage of the total number of cells.
               for(int i = 0;i < total_wall_number;i++){ // loop to place the walls 
                                      int x,y; // variables to store the x co-ordinate and y co-ordinate.
                                      do {
                                           x =  random_number_generator(0, grid->N -1); // uses the random_number_generator function to generate a random x co-ordinate.
                                           y =  random_number_generator(0, grid->N -1); // uses the random_number_generator function to generate a random y co-ordinate.
                                         } while(grid->grid[x][y] != Empty_Space); // this loop continues to generate random values of x and y values until the location in the grid is not an empty space
                                      grid->grid[x][y] = Wall; // Once the random x and y co-ordinates have been successfully generated,a wall is tha placed at that location in the grid.
              }
}
void place_items_randomly_2player(Grid *grid,Player players[2]) {


               do {
                    grid->extraction.x = random_number_generator(0, grid->N -1); // generates a random x co-ordinate for the Extraction Point.            
                    grid->extraction.y = random_number_generator(0, grid->N -1);    // generates a random y co-ordinate for the Extraction Point. 
               } while(grid->grid[grid->extraction.x][grid->extraction.y] != Empty_Space); //this loop continues to generate random values of x and y values until the location in the grid is not an empty space
               grid->grid[grid->extraction.x][grid->extraction.y] = Extraction_Point; // // Once the random x and y co-ordinates have been successfully generated,an extraction point is tha placed at that location in the grid.

              /* same logic is applied to place Intels*/
               for(int i = 0;i < MAX_INTEL_COLLECTABLE;i++){
                                      int x,y;
                                      do {
                                            x = random_number_generator(0, grid->N -1);
                                            y = random_number_generator(0, grid->N -1);
                                      } while (grid->grid[x][y] != Empty_Space);
                                      grid->grid[x][y] = Intel;
               }

               /* same logic is applied to place Lives*/

               for(int j = 0;j < MAX_LIVES_COLLECTABLE;j++){
                                      int x,y;
                                      do {
                                            x = random_number_generator(0, grid->N -1);
                                            y = random_number_generator(0, grid->N -1);
                                      } while (grid->grid[x][y] != Empty_Space);
                                      grid->grid[x][y] = Lives;
               }

               /* same logic is applied to generate and place the starting point for the Player1 and Player2*/
               do {
                    players[0].position.x = random_number_generator(0, grid->N -1);
                    players[0].position.y = random_number_generator(0, grid->N -1);
               } while(grid->grid[players[0].position.x][players[0].position.y] != Empty_Space);
               grid->grid[players[0].position.x][players[0].position.y] = Player1_Symbol; // Once the co-ordinates of Player1 is decided,Player's symbol is placed at the position.
              
               do {
                    players[1].position.x = random_number_generator(0, grid->N -1);
                    players[1].position.y = random_number_generator(0, grid->N -1);
               } while(grid->grid[players[1].position.x][players[1].position.y] != Empty_Space || (players[1].position.x == players[0].position.x && players[1].position.y == players[0].position.y)); // checks whether the position of Player 2 is not an Empty Space and does not contradict with the position of Player 1.
               grid->grid[players[1].position.x][players[1].position.y] = Player2_Symbol; // Once the co-ordinates of Player2 is decided,Player 2's symbol is placed at the position.

 }
int initialise_game_2player(GameState *game, int grid_size,int player2_is_computer) {
    // Create grid
    game->grid = create_grid(grid_size); // Creates the grid for the game to be played
    if (game->grid == NULL) {
        printf("Failed to allocate memory for grid!\n");
        return 0;  // // returns a negative acknowledgement that the game has been successfully created.
        
    }

    /* Player 1 states are initialised accordingly*/
    game->players[0].total_lives = 3;
    game->players[0].intel_collected = 0;
    game->players[0].is_active = 1;
    game->players[0].moves_count = 0;
    game->players[0].symbol = Player1_Symbol;
    game->players[0].is_computer = 0;  // Player is always human
    
    /* Player 2 states are initialised accordingly*/
    game->players[1].total_lives = 3;
    game->players[1].intel_collected = 0;
    game->players[1].is_active = 1;
    game->players[1].moves_count = 0;
    game->players[1].symbol = Player2_Symbol;
    game->players[1].is_computer = player2_is_computer;  //Based on user's choice this member's value alters.
   
    /* game state is initialised as follows. */
    game->current_player = 0;      // Player 1 always starts
    game->game_terminated = 0;     // indicates that the game is not terminated.
    game->winner = -1;             // '-1' indicates that there are no winners,presently,can be later modified
    game->active_players = 2; // shows that there are two active players currently.

    generate_walls(game->grid); // Walls are generated in the game grid by calling this function.


    place_items_randomly_2player(game->grid, game->players); // places items and starting position of the player by calling this function.

    printf("Game initialized with %d x %d grid\n", grid_size, grid_size); // acknowledges that the game grid has been created with the specified dimensions.
    return 1;  // returns a positive acknowledgement that the game has been successfully created.
}

int get_grid_size(){
              int N; // a variable to store Grid Size
              while(1) {
                    printf("Enter a grid size from range : %d to %d: ",MIN_GRID_SIZE,MAX_GRID_SIZE); // allows the player to choose a Grid Size to play with.
                    if(scanf("%d",&N) != 1){ // checks if input by user is an inteer
                                               printf("Invalid Input! Please enter a number.\n"); // displays an error message if not an integer.
                                               while(getchar() != '\n'); // clears the buffer
                                               continue;
                    }
                    while(getchar() != '\n'); // clears the entire line

                    if(N >= MIN_GRID_SIZE && N<= MAX_GRID_SIZE) // check if the input by user is between the mininum and maximum grid size...
                                               return N; // if yes,returns the user's choice of grid size.
                    printf("Invalid size!\n"); // if no,prints an error message!
              }
}
void update_player_state_2player(GameState *game,char direction){
 
              Player *current = &game->players[game->current_player]; // the variable 'current' points to the current player.
              int old_x_coordinate = current -> position.x; // a variable that stores the old value of x.
              int old_y_coordinate = current -> position.y; // a variable that stores the old value of y.
              /* iniltialises the new values of x and y to the old values of x and y because only one of the two is altered as a result of user input.*/
              int new_x_coordinate = old_x_coordinate;
              int new_y_coordinate = old_y_coordinate;
              switch(direction){ // checks the direction user entered.
                     case 'W' :
                              new_x_coordinate = old_x_coordinate - 1; // if 'W' is chosen by player then the player's position is moved upwards in the 2D Grid.
                              break;
                     case 'A' :
                              new_y_coordinate = old_y_coordinate - 1; // if 'A' is chosen by player then the player's position is moved left in the 2D Grid.
                              break;
                     case 'S' :
                              new_x_coordinate = old_x_coordinate + 1; // if 'S' is chosen by player then the player's position is moved downwards in the 2D Grid.
                              break;
                     case 'D' :
                              new_y_coordinate = old_y_coordinate + 1; // if 'D' is chosen by player then the player's position is moved right in the 2D Grid.
                              break;
              }


              char new_position  = game->grid->grid[new_x_coordinate][new_y_coordinate]; // the new cell's  item in the Game Grid is stored in the variable.
               if(game->grid->grid[old_x_coordinate][old_y_coordinate] != Extraction_Point) // check if the old location in the Game Grid wasn't a Extraction Point.(X)
                                              game->grid->grid[old_x_coordinate][old_y_coordinate] = Empty_Space; // if yes then,an empty space is added since the player has left that position.

              /* in the position structure,the new x and y values of the Player is updated.*/
                 current->position.x = new_x_coordinate;
                 current->position.y = new_y_coordinate;
              if(new_position == Intel || new_position == Lives) //checks if there is an Intel or a Life in the new position
                                             collect_item_2player(game,new_x_coordinate,new_y_coordinate); // calls the function collect_item to collect the item

              if (new_position != Extraction_Point) // check if the new position is not an Extraction Point(X)
                                             game->grid->grid[new_x_coordinate][new_y_coordinate] = current -> symbol; // places the Player_Symbol to mark that the new position of the player in the game,



              printf("Moved %c to position ( %d , %d ) in the grid\n",direction,new_x_coordinate,new_y_coordinate); // notifies the user of their current location in the grid and the movement they decided to take.
}
int choose_game_mode() {
              int choice; // a variable to store whether the player wishes to play with another player or a computer.
              /* Displays the selection for the user to choose from,either : 1 or 2 can chosen by player.*/
              printf("=== SELECT GAME MODE ===\n");
              printf("1. Human vs Human\n");
              printf("2. Human vs Computer\n");
              printf("===        ===        ===\n");
             while(1) {
                      printf("Enter your choice!\n"); 
                      if(scanf("%d",&choice) != 1) { // checks if the user input is valid(integer)
                                               printf("Invalid Input for choice!\nYou have not entered an integer!\n"); // if invalid,prints an error message!
                                               while (getchar() != '\n'); // Clears the buffer
                                               continue; // proceeds to the next iteration of the while loop to allow the player to choose an option again.
                      }
                      if(choice != 1 && choice != 2) {
                                             printf("Invalid choice! Please enter only:  1 or 2.\n"); 
                                            
                                             continue; // proceeds to the next iteration of the while loop to allow the player to choose an option again.
                     }  else {
                                             while (getchar() != '\n'); // Clears the buffer
                                             break; // proceeds to the next iteration of the while loop to allow the player to choose an option again.
                           }
             }
             return choice;
}
char get_computer_move() {
    char moves[] = {'W', 'A', 'S', 'D'}; // stores the symbols for movements in an array.
    return moves[rand() % 4]; // a random number between 0 to 3 is generated and that element in that index of the array is returned.
}
