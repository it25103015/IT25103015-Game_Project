#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>


#define MIN_GRID_SIZE 5
#define MAX_GRID_SIZE 15
#define MAX_INTEL_COLLECTABLE 3
#define MAX_LIVES_COLLECTABLE 2
#define PLAYER_SYMBOL '@'
#define WALL '#'
#define INTEL 'I'
#define LIVES 'L'
#define EXTRACTION_POINT 'X'
#define EMPTY_SPACE '.'
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
int initialise_game(GameState *game,int grid_size);// starts the game
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
void display_movement_instructions();
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
                              display_movement_instructions(); // displays the movement instructions again as a reminder for the user.
                              char move = get_movement_key(); // gets the movement key inputted by the user and stores it in variable ' move'
                              move = toupper(move); // converts the movement key input to uppercase in case user prefers to enter lowercase letters
                              if(move == 'Q') { // checks if the users prefers to quit the game
                                              printf("You have terminated the game!\n"); // displays of acknowledgement of user's choice to end the game and subsequently exits the game.
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

void display_movement_instructions(){
    printf("These are the movement keys that you need to type to navigate through the grid:\n");
    printf("  'W' to move up by one cell.\n");
    printf("  'A' to move left by one cell.\n");
    printf("  'S' to move down by one cell.\n");
    printf("  'D' to move right by one cell.\n");
    printf("  'Q' to quit the game.\n\n");
    printf("  '%c' - Your position in the grid!\n", PLAYER_SYMBOL);
    printf("  '%c' - Wall (You need to avoid these!)\n", WALL);
    printf("  '%c' - Intel (collect all 3 of these in the grid to win)\n", INTEL);
    printf("  '%c' - Extra Life (collect to increase lives)\n", LIVES);
    printf("  '%c' - Extraction Point (reach after collecting all Intel)\n", EXTRACTION_POINT);
    printf("  '%c' - Empty space (safe to move through)\n\n", EMPTY_SPACE);
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
    printf("  '%c' - Your position in the grid!\n", PLAYER_SYMBOL);
    printf("  '%c' - Wall (You need to avoid these!)\n", WALL);
    printf("  '%c' - Intel (collect all 3 of these in the grid to win)\n", INTEL);
    printf("  '%c' - Extra Life (collect to increase lives)\n", LIVES);
    printf("  '%c' - Extraction Point (reach after collecting all Intel)\n", EXTRACTION_POINT);
    printf("  '%c' - Empty space (safe to move through)\n\n", EMPTY_SPACE);
    
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
                      if(game->grid->grid[new_x][new_y] == WALL) // check if user has collided with a wall;
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
                                   case INTEL :
                                              game->grid->Intel_Remaining--; // if Intel then the number of Intels to be displayed in the game grid decreases by 1.
                                              game->player.intel_collected++; // if Intel then the number of Intels collected by the player increases by 1/
                                              printf("You have collected an Intel! The total of collected intel is :%d \n",game->player.intel_collected); // notifies the player that they have successfully collected an intel.
                                              break;
                                   case LIVES :
                                              game->grid->Lives_Remaining--; // if Life then the number of Lives to be displayed in the game grid decreases by 1
                                              game->player.total_lives++; // if Life then the total number of Lives collected by the Player increases by 1.
                                              printf("You have collected a live! You have a total of : %d\n",game-> player.total_lives); // notifies the player the total number of lives they have.
                                              break;
                                   }
                                   if (Item == INTEL || Item == LIVES) 
                                                                   game->grid->grid[x][y] = EMPTY_SPACE; // removes the item after it is collected by the player.
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
                                           grid->grid[i][j] = EMPTY_SPACE; // initialises all cells in this row to Empty_Space symbol.
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

void log_game_state(GameState *game,char* Action_Description) {
                                       FILE *fptr = fopen(LOG_FILE,"a"); // creates a file pointer that points to the memory address of the file being accessed.
                                       if(fptr == NULL){ // checks if the file pointer is valid
                                                     printf("Error opening file!\n"); // displays error message if the file pointer is invalid.
                                                     return;
                                       }
                                       fprintf(fptr,"\nMove Number : %d\n",game->player.moves_count + 1); // writes the current move number of the the player,
                                       fprintf(fptr,"Action : %s\n",Action_Description); // write the current Action that was chosen by the Player
                                       fprintf(fptr,"Player's position is : (%d , %d)\n",game->player.position.x,game->player.position.y); // writes the current player's co-ordinates in the grid.
                                       fprintf(fptr,"Player has a total of %d lives\n",game->player.total_lives); // writes the total number of lives the player currently has.
                                       fprintf(fptr,"Intel Collected : %d/%d\n",game->player.intel_collected,MAX_INTEL_COLLECTABLE); // writes the number of intels collected as a fraction of the total.
                                       
                                       
                                       for(int i = 0;i < game->grid->N;i++){
                                                          for(int j = 0;j < game->grid->N;j++) 
                                                                         fprintf(fptr,"%c ",game->grid->grid[i][j]); // writes the state of the grid(location of life,intel etc...)
                                                          fprintf(fptr,"\n");
                                       }
                                       
                                       fclose(fptr);
}
void check_win_conditions(GameState *game){
                int Is_Extraction_Point; // a variable to indicate whether player has reached the extraction point.'0' for no and '1' for yes.
                Is_Extraction_Point = (game->player.position.x == game->grid->extraction.x) && (game->player.position.y == game->grid->extraction.y); // if the current x and y co-ordinates of the player in the grid is the extraction point then '1' is put to the Is_Extraction_Point or '0' if not the extraction poitn.
                if(Is_Extraction_Point == 1){ // if the player has reached the extraction point
                                            if(game->player.intel_collected == MAX_INTEL_COLLECTABLE){ // checks if the player has collected all the intel to secure victory.
                                                                            game->game_terminated = 1; // updates the game's state to terminated.
                                                                            game-> game_won = 1; // indicates that the player has won the game.
                                                                            game-> player.is_active = 0; // updates the player's state to inactive.
                                            } else { 
                                                    game->game_terminated = 1; // updates the game's state to terminated.
                                                    game-> game_won = 0; // since the user has not collected all the intels,the player has lost the game.
                                                    game-> player.is_active = 0; // updates the player's state to inactive.
                                                   }
               } else if (game->player.total_lives <= 0){ // if the user has run out of lives.
                                                        game->game_terminated = 1; // game is terminated because player cannot play wothout lives
                                                        game-> game_won = 0; // indicates the user has lost the game.
                                                        game-> player.is_active = 0; // indicates that the player is inactive.
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




void show_game_results(GameState *game){
               /* It shows the results of the game,whether the player has won or lost the game.*/
               printf("\n=== Game Over! ===\n");
               if(game->game_won == 1) {
                              printf("=== VICTORY!=== \n Congratulations you have successfully completed the mission!\n");
                              printf("Your progress has been saved to %s\nPlease check that out and always be a victor!\n",LOG_FILE);
               }
               else {
                              printf("=== DEFEAT! === \nMission unsuccessful...\nBetter luck next time!\n");
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
                                         } while(grid->grid[x][y] != EMPTY_SPACE); // this loop continues to generate random values of x and y values until the location in the grid is not an empty space
                                      grid->grid[x][y] = WALL; // Once the random x and y co-ordinates have been successfully generated,a wall is tha placed at that location in the grid.
              }
}

void place_items_randomly(Grid *grid,Position *player_start) {
              
                
               do {
                    grid->extraction.x = random_number_generator(0, grid->N -1); // generates a random x co-ordinate for the Extraction Point.            
                    grid->extraction.y = random_number_generator(0, grid->N -1);    // generates a random y co-ordinate for the Extraction Point. 
               } while(grid->grid[grid->extraction.x][grid->extraction.y] != EMPTY_SPACE); //this loop continues to generate random values of x and y values until the location in the grid is not an empty space
               grid->grid[grid->extraction.x][grid->extraction.y] = EXTRACTION_POINT; // // Once the random x and y co-ordinates have been successfully generated,an extraction point is tha placed at that location in the grid.
              
              /* same logic is applied to place Intels*/
               for(int i = 0;i < MAX_INTEL_COLLECTABLE;i++){ 
                                      int x,y;
                                      do {
                                            x = random_number_generator(0, grid->N -1);
                                            y = random_number_generator(0, grid->N -1);
                                      } while (grid->grid[x][y] != EMPTY_SPACE);
                                      grid->grid[x][y] = INTEL;
               }
               
               /* same logic is applied to place Lives*/

               for(int j = 0;j < MAX_LIVES_COLLECTABLE;j++){
                                      int x,y;
                                      do {
                                            x = random_number_generator(0, grid->N -1);
                                            y = random_number_generator(0, grid->N -1);
                                      } while (grid->grid[x][y] != EMPTY_SPACE);
                                      grid->grid[x][y] = LIVES;
               }

               /* same logic is applied to generate and place the starting point for the Player*/
               do {
                    player_start->x = random_number_generator(0, grid->N -1);                  
                    player_start->y = random_number_generator(0, grid->N -1);    
               } while(grid->grid[player_start->x][player_start->y] != EMPTY_SPACE);    
           
 }              
int initialise_game(GameState *game, int grid_size) {
    // Create grid
    game->grid = create_grid(grid_size); // Creates the grid for the game to be played
    if (game->grid == NULL) {
        printf("Failed to allocate memory for grid!\n");
        return 0;  // // returns a negative acknowledgement that the game has been successfully created.

    }
    
    /* Player states are initialised accordingly*/
    game->player.total_lives = 3;
    game->player.intel_collected = 0;
    game->player.is_active = 1;
    game->player.moves_count = 0;
    
    
    generate_walls(game->grid); // Walls are generated in the game grid by calling this function.
    
    
    place_items_randomly(game->grid, &game->player.position); // places items and starting position of the player by calling this function.
    
    
    game->grid->grid[game->player.position.x][game->player.position.y] = PLAYER_SYMBOL; // Places player's symbol at the the starting position.
    
    /* Initialize game state flags accordingly */
    game->game_terminated = 0;
    game->game_won = 0;
    
    printf("✓ Game initialized with %d x% d grid\n", grid_size, grid_size); // acknowledges that the game grid has been created with the specified dimensions.
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

void update_player_state(GameState *game,char direction){
              int old_x_coordinate = game->player.position.x; // a variable that stores the old value of x.
              int old_y_coordinate = game->player.position.y; // a variable that stores the old value of y.
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
               if(game->grid->grid[old_x_coordinate][old_y_coordinate] != EXTRACTION_POINT) // check if the old location in the Game Grid wasn't a Extraction Point.(X)
                                              game->grid->grid[old_x_coordinate][old_y_coordinate] = EMPTY_SPACE; // if yes then,an empty space is added since the player has left that position.

              /* in the position structure,the new x and y values of the Player is updated.*/
              game->player.position.x = new_x_coordinate; 
              game->player.position.y = new_y_coordinate;
              if(new_position == INTEL || new_position == LIVES) //checks if there is an Intel or a Life in the new position
                                             collect_item(game,new_x_coordinate,new_y_coordinate); // calls the function collect_item to collect the item

              if (new_position != EXTRACTION_POINT) // check if the new position is not an Extraction Point(X)
                                             game->grid->grid[new_x_coordinate][new_y_coordinate] = PLAYER_SYMBOL; // places the Player_Symbol to mark that the new position of the player in the game,
              
            
              
              printf("Moved %c to position ( %d , %d ) in the grid\n",direction,new_x_coordinate,new_y_coordinate); // notifies the user of their current location in the grid and the movement they decided to take.
}


