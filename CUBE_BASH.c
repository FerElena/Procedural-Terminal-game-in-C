#include "CUBE_BASH.h"

// Global Variables
ghost Main_Ghost;
rope Main_rope;
static unsigned char Screen[SCREEN_HEIGHT][SCREEN_WIDE]; // Matrix where the game is being played
unsigned char *Lastline_ptr = Screen[SCREEN_HEIGHT - 1]; // Pointer to write messages on the last line
static struct termios old_termios, new_termios;          // For configuring terminal on non-canonical mode, and no echo
unsigned int num_cubes = 0;
unsigned int min_cubes = 0;
unsigned int max_cubes = 0;
unsigned int hitted_cubes = 0;

// Screen Functions
void SC_moveCursor(unsigned int x, unsigned int y)
{
    // Move cursor to the specified coordinates
    if (x >= SCREEN_WIDE || y >= SCREEN_HEIGHT)
    {
        // Anti-self-check
        printf("NOPEEEEEEEEEEEEEEEEEEEEEE\n");
    }
    else
    {
        printf("\033[%d;%dH", x, y);
    }
}

void SC_renderTitle(int render_time)
{
    // Renders the title screen with specified render time
    printf("\033[2J"); // Clear screen
    SC_moveCursor(1, 1);
    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < SCREEN_WIDE; j++)
        {
            if (i == 0 || j == 0 || j == SCREEN_WIDE - 2 || i == SCREEN_HEIGHT - 2)
            {
                printf("\033[0;31m"); // Red color for borders
                putchar(Screen[i][j]);
                printf("\033[0m");
                continue;
            }
            else if (Screen[i][j] == 'O')
            {
                printf("\033[1;32m"); // Green color for 'O'
                putchar(Screen[i][j]);
                printf("\033[0m");
                continue;
            }
            else if (Screen[i][j] == '(' || Screen[i][j] == ')')
            {
                printf("\033[1;34m"); // Blue color for '(' and ')'
                putchar(Screen[i][j]);
                printf("\033[0m");
                continue;
            }
            else if (i < 11)
            {
                printf("\033[1;35m"); // Magenta color for lines above line 11
                putchar(Screen[i][j]);
                printf("\033[0m");
                continue;
            }
            else if (Screen[i][j] == '-' || Screen[i][j] == '>' || Screen[i][j] == '<')
            {
                printf("\033[1;34m"); // Blue color for '-', '>', and '<'
                putchar(Screen[i][j]);
                printf("\033[0m");
                continue;
            }
            printf("\033[0;35m"); // Magenta color for other characters
            putchar(Screen[i][j]);
            printf("\033[0m");
        }
        if (render_time)
        {
            usleep(50000); // Sleep for smooth rendering
        }
    }
    fflush(stdout);
}

void SC_renderscreen()
{
    // Renders the game screen
    SC_moveCursor(1, 1);
    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < SCREEN_WIDE; j++)
        {
            if (i == 0 || (j == 0 && i != SCREEN_HEIGHT - 1) || j == SCREEN_WIDE - 2 || i == SCREEN_HEIGHT - 2)
            {
                printf("\033[0;31m"); // Red color for borders
                putchar(Screen[i][j]);
                printf("\033[0m");
                continue;
            }
            else if (Screen[i][j] == 'O' || (Screen[i][j] == 'o' && i != SCREEN_HEIGHT - 1))
            {
                printf("\033[1;32m"); // Green color for 'O' and 'o' except the last line
                putchar(Screen[i][j]);
                printf("\033[0m");
                continue;
            }
            else if (Screen[i][j] == '(' || Screen[i][j] == ')' || Screen[i][j] == '-' || Screen[i][j] == '.')
            {
                printf("\033[1;34m"); // Blue color for '(', ')', '-', and '.'
                putchar(Screen[i][j]);
                printf("\033[0m");
                continue;
            }
            else if (Screen[i][j] == 'U' || Screen[i][j] == '\"' || Screen[i][j] == '\'')
            {
                printf("\033[0;35m"); // Magenta color for 'U', '\"', and '\''
                putchar(Screen[i][j]);
                printf("\033[0m");
                continue;
            }
            else if (Screen[i][j] == '=' || Screen[i][j] == '|')
            {
                printf("\033[1;35m"); // Magenta color for '=' and '|'
                putchar(Screen[i][j]);
                printf("\033[0m");
                continue;
            }
            else if (Screen[i][j] == '+')
            {
                printf("\033[1;33m"); // Yellow color for '+'
                putchar(Screen[i][j]);
                printf("\033[0m");
                continue;
            }
            else if (Screen[i][j] == '*')
            {
                printf("\033[1;34m"); // Blue color for '*'
                putchar(Screen[i][j]);
                printf("\033[0m");
                continue;
            }
            else if (i == SCREEN_HEIGHT - 1)
            {
                printf("\033[1;31m"); // Red color for the last line
                putchar(Screen[i][j]);
                printf("\033[0m");
                continue;
            }
            putchar(Screen[i][j]);
        }
    }
    fflush(stdout);
}

void SC_clean_character(unsigned char char_in)
{
    // Cleans the specified character from the screen
    for (int i = 1; i < SCREEN_HEIGHT - 2; i++)
    {
        for (int j = 0; j < SCREEN_WIDE - 2; j++)
        {
            if (Screen[i][j] == char_in)
                Screen[i][j] = ' ';
        }
    }
}

void SC_Title_Screen()
{
    // Renders the title screen and handles user input
    int rendered_title = 1, input_key;
    memcpy(Screen, Title_Screen1, sizeof(Title_Screen1));
    SC_renderTitle(SLOW_RENDER);
    clear_stdin_buffer();

    while (1)
    {
        if (rendered_title)
        {
            memcpy(Screen, Title_Screen1, sizeof(Title_Screen1));
            rendered_title = 0;
        }
        else
        {
            memcpy(Screen, Title_Screen2, sizeof(Title_Screen2));
            rendered_title = 1;
        }
        sleep(1);
        input_key = KI_read_input();
        if (input_key == 7)
        {
            break; // Exit loop if the user presses the enter key
        }
        SC_renderTitle(FAST_RENDER);
    }
}

void SC_Game_over()
{
    // Renders the "Game Over" screen
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 115; j++)
        {
            Screen[15 + i][j + 5] = game_over[i][j];
        }
    }
}

void SC_rendergameover()
{
    // Renders the "Game Over" screen with dark gray color
    SC_moveCursor(1, 1);
    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < SCREEN_WIDE; j++)
        {
            printf("\033[1;30m"); // Dark gray color
            putchar(Screen[i][j]);
        }
    }
    fflush(stdout);
}


//=================================================TERMINAL FUNCTIONS=========================================================================================

// Resets the terminal to its pre-game state
void TR_reset_terminal()
{
    printf("\e[m");    // Reset color changes
    printf("\e[?25h"); // Show the cursor again
    printf("\033[2J"); // Clear the screen
    fflush(stdout);
    SC_moveCursor(1, 1);
    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios); // Restore old terminal setup
}

// Configures the terminal for no echo to prevent it from echoing characters typed,
// and sets non-canonical mode to read input instantly
void TR_configure_Terminal()
{
    tcgetattr(STDIN_FILENO, &old_termios);
    new_termios = old_termios;

    new_termios.c_lflag &= ~(ICANON | ECHO); // Turn off echo in terminal, and set non-canonical mode
    new_termios.c_cc[VMIN] = 0;
    new_termios.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
    printf("\033[?25l"); // Hide the cursor
    atexit(TR_reset_terminal);
}

//===============================================================HANDLE ERROR================================================================================================================

void handle_error(unsigned char *error_message)
{
    // Reset terminal to its default state and print error message before exiting
    TR_reset_terminal();
    printf("%s\n", error_message);
    sleep(5);
    exit(0);
}


//================================================KEY INPUT FUNCTIONS=======================================================================================================

int KI_read_key(char *buff, int k)
{
    // Returns an integer code depending on the admitted keys for the game
    char allowed_keys[] = {'a', 'w', 's', 'd', 'p', 'm', '\n'};
    for (int i = 0; i < sizeof(allowed_keys); i++)
    {
        if (buff[k] == allowed_keys[i])
        {
            return i + 1; // Return a unique value for each key
        }
    }
    return 0; // Not a valid key
}

int KI_read_input()
{
    char in_buff[4096];
    int n = read(STDIN_FILENO, in_buff, sizeof(in_buff));
    int final_key = 0;
    for (int k = 0; k < n; k++)
    {
        int key = KI_read_key(in_buff, k);
        if (key != 0)
        {
            final_key = key;
            break; // Only need one valid key
        }
    }
    return final_key;
}

void clear_stdin_buffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ; // Read and discard characters until finding a new line or end of file
}

//================================================MAIN CHARACTER FUNCTIONS==================================================================

unsigned int MC_checkspawn(int y, int x)
{
    // Check if a randomly generated spawn-point is a valid spawn-point
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            if (Screen[i + y][j + x] != ' ')
                return 0;
        }
    }
    return 1;
}

void MC_renderghost(int y, int x, int dir)
{
    // Renders the ghost in the input coordinates
    switch (dir)
    {
    case 0:
        for (int i = 0; i < 3; i++)
        {
            memcpy(Screen[y + i] + x, ghost_ascii1[i], 6);
        }
        break;
    case 1:
        for (int i = 0; i < 3; i++)
        {
            memcpy(Screen[y + i] + x, ghost_ascii2[i], 6);
        }
        break;
    case 2:
        for (int i = 0; i < 3; i++)
        {
            memcpy(Screen[y + i] + x, ghost_ascii3[i], 6);
        }
        break;
    case 3:
        for (int i = 0; i < 3; i++)
        {
            memcpy(Screen[y + i] + x, ghost_ascii4[i], 6);
        }
        break;
    }

    Main_Ghost.y = y;
    Main_Ghost.x = x;
}

void MC_spawn_Ghost(unsigned int seed)
{
    // Spawns a ghost in the screen depending on a seed, if it fails, it tries again on a different position in the Screen matrix
    int y, x;
    for (;;)
    {
        y = GF_generate_pseudorandom_number(seed, 3, SCREEN_HEIGHT - 5);
        x = GF_generate_pseudorandom_number(seed, 1, SCREEN_WIDE - 8);
        int valid = MC_checkspawn(y, x);
        if (valid)
        {
            break;
        }
        seed = GF_generate_pseudorandom_number(seed, 0, 100000);
    }
    MC_renderghost(y, x, 3);
}

int MC_Isvalid_move(int y, int x, int move)
{
    // Checks if an input move is valid
    switch (move)
    {
    case LEFT + 1:
        for (int i = 0; i < 3; i++)
        {
            if (Screen[y + i][x - 1] == '|' || Screen[y + i][x - 1] == '+')
                return 0;
        }
        break;
    case UP + 1:
        for (int i = 0; i < 6; i++)
        {
            if (Screen[y - 1][x + i] == '=' || Screen[y - 1][x + i] == '+')
                return 0;
        }
        break;
    case DOWN + 1:
        for (int i = 0; i < 6; i++)
        {
            if (Screen[y + 3][x + i] == '=' || Screen[y + 3][x + i] == '+')
                return 0;
        }
        break;
    case RIGHT + 1:
        for (int i = 0; i < 3; i++)
        {
            if (Screen[y + i][x + 6] == '|' || Screen[y + i][x + 6] == '+')
                return 0;
        }
        break;
    default:
        return 0;
        break;
    }
    return 1;
}

// Function to check if there is a collision with an obstacle or rope at the specified coordinates on the screen
int check_collision_rope(unsigned int y, unsigned int x)
{
    // Checks if the character at the specified coordinates is neither empty nor an obstacle or rope
    if (Screen[y][x] != ' ' && Screen[y][x] != '+' && Screen[y][x] != '*')
    {
        return 0; // Collision detected
    }
    return 1; // No collision
}

// Function to validate shooting in a given direction
int MC_validshoot(unsigned int y, unsigned int x, unsigned int direction)
{
    // Checks if shooting in the specified direction is possible without collision
    switch (direction)
    {
    case LEFT:
        return check_collision_rope(y, x - 1);
    case UP:
        return check_collision_rope(y - 1, x);
    case DOWN:
        return check_collision_rope(y + 1, x);
    case RIGHT:
        return check_collision_rope(y, x + 1);
    default:
        return 0;
    }
}

// Function to spawn a rope
int MC_spawn_rope()
{
    // Spawns a rope based on the direction of the main ghost
    if (Main_Ghost.dir == LEFT || Main_Ghost.dir == RIGHT)
    {
        if (Main_Ghost.dir == LEFT)
        {
            if (MC_validshoot(Main_Ghost.y + 1, Main_Ghost.x, LEFT) && Screen[Main_Ghost.y + 1][Main_Ghost.x - 1] != '+')
            {
                Main_rope.y = Main_Ghost.y + 1;
                Main_rope.x = Main_Ghost.x - 1;
                Main_rope.dir = LEFT;
                Screen[Main_rope.y][Main_rope.x] = '*';
                return 1;
            }
        }
        else
        {
            if (MC_validshoot(Main_Ghost.y + 1, Main_Ghost.x + 5, RIGHT) && Screen[Main_Ghost.y + 1][Main_Ghost.x + 6] != '+')
            {
                Main_rope.y = Main_Ghost.y + 1;
                Main_rope.x = Main_Ghost.x + 6;
                Main_rope.dir = RIGHT;
                Screen[Main_rope.y][Main_rope.x] = '*';
                return 1;
            }
        }
    }
    else
    {
        if (Main_Ghost.dir == UP)
        {
            if (Main_Ghost.last_dir_x == LEFT)
            {
                if (MC_validshoot(Main_Ghost.y, Main_Ghost.x + 1, UP) && Screen[Main_Ghost.y - 1][Main_Ghost.x + 1] != '+')
                {
                    Main_rope.y = Main_Ghost.y - 1;
                    Main_rope.x = Main_Ghost.x + 1;
                    Main_rope.dir = UP;
                    Screen[Main_rope.y][Main_rope.x] = '*';
                    return 1;
                }
            }
            else
            {
                if (MC_validshoot(Main_Ghost.y, Main_Ghost.x + 4, UP) && Screen[Main_Ghost.y - 1][Main_Ghost.x + 4] != '+')
                {
                    Main_rope.y = Main_Ghost.y - 1;
                    Main_rope.x = Main_Ghost.x + 4;
                    Main_rope.dir = UP;
                    Screen[Main_rope.y][Main_rope.x] = '*';
                    return 1;
                }
            }
        }
        else
        {
            if (Main_Ghost.last_dir_x == LEFT)
            {
                if (MC_validshoot(Main_Ghost.y - 2, Main_Ghost.x + 1, DOWN) && Screen[Main_Ghost.y + 3][Main_Ghost.x + 1] != '+')
                {
                    Main_rope.y = Main_Ghost.y + 3;
                    Main_rope.x = Main_Ghost.x + 1;
                    Main_rope.dir = DOWN;
                    Screen[Main_rope.y][Main_rope.x] = '*';
                    return 1;
                }
            }
            else
            {
                if (MC_validshoot(Main_Ghost.y - 2, Main_Ghost.x + 4, DOWN) && Screen[Main_Ghost.y + 3][Main_Ghost.x + 4] != '+')
                {
                    Main_rope.y = Main_Ghost.y + 3;
                    Main_rope.x = Main_Ghost.x + 4;
                    Main_rope.dir = DOWN;
                    Screen[Main_rope.y][Main_rope.x] = '*';
                    return 1;
                }
            }
        }
    }
    return 0;
}

// Function to handle the 90-degree bounce of the rope when it hits a wall
int MC_bounce90rope()
{
    // Handles the bounce of the rope when it hits a wall at a 90-degree angle
    if (Main_rope.dir == LEFT)
    {
        if (Screen[Main_rope.y + 1][Main_rope.x - 1] == '|')
        {
            if (check_collision_rope(Main_rope.y - 1, Main_rope.x - 1))
            {
                Main_rope.y--;
                Main_rope.x--;
                Main_rope.dir = UP;
                Screen[Main_rope.y][Main_rope.x] = '*';
                return 1;
            }
        }
        else
        {
            if (check_collision_rope(Main_rope.y + 1, Main_rope.x - 1))
            {
                Main_rope.y++;
                Main_rope.x--;
                Main_rope.dir = DOWN;
                Screen[Main_rope.y][Main_rope.x] = '*';
                return 1;
            }
        }
    }
    else if (Main_rope.dir == RIGHT)
    {
        if (Screen[Main_rope.y + 1][Main_rope.x + 1] == '|')
        {
            if (check_collision_rope(Main_rope.y - 1, Main_rope.x + 1))
            {
                Main_rope.y--;
                Main_rope.x++;
                Main_rope.dir = UP;
                Screen[Main_rope.y][Main_rope.x] = '*';
                return 1;
            }
        }
        else
        {
            if (check_collision_rope(Main_rope.y + 1, Main_rope.x + 1))
            {
                Main_rope.y++;
                Main_rope.x++;
                Main_rope.dir = DOWN;
                Screen[Main_rope.y][Main_rope.x] = '*';
                return 1;
            }
        }
    }
    else if (Main_rope.dir == UP)
    {
        if (Screen[Main_rope.y - 1][Main_rope.x - 1] == '=')
        {
            if (check_collision_rope(Main_Ghost.y - 1, Main_Ghost.x + 1))
            {
                Main_rope.y--;
                Main_rope.x++;
                Main_rope.dir = RIGHT;
                Screen[Main_rope.y][Main_rope.x] = '*';
                return 1;
            }
        }
        else
        {
            if (check_collision_rope(Main_rope.y - 1, Main_rope.x - 1))
            {
                Main_rope.y--;
                Main_rope.x--;
                Main_rope.dir = LEFT;
                Screen[Main_rope.y][Main_rope.x] = '*';
                return 1;
            }
        }
    }
    else if (Main_rope.dir == DOWN)
    {
        if (Screen[Main_rope.y + 1][Main_rope.x - 1] == '=')
        {
            if (check_collision_rope(Main_Ghost.y + 1, Main_Ghost.x + 1))
            {
                Main_rope.y++;
                Main_rope.x++;
                Main_rope.dir = RIGHT;
                Screen[Main_rope.y][Main_rope.x] = '*';
                return 1;
            }
        }
        else
        {
            if (check_collision_rope(Main_rope.y + 1, Main_rope.x - 1))
            {
                Main_rope.y++;
                Main_rope.x--;
                Main_rope.dir = LEFT;
                Screen[Main_rope.y][Main_rope.x] = '*';
                return 1;
            }
        }
    }
    return 0; // No bounce occurred
}


// Function to advance the rope
int MC_advance_rope()
{
    static unsigned int turn = 0; // Variable to keep track of the rope's turn
    switch (Main_rope.dir)
    {
    case LEFT:
        if (MC_validshoot(Main_rope.y, Main_rope.x, Main_rope.dir))
        {
            if (Screen[Main_rope.y][Main_rope.x - 1] == '+') // Checks for collision with a cube
            {
                if (MC_bounce90rope()) // Handles 90-degree bounce
                {
                    hitted_cubes++; // Increment hitted cubes count
                    return 1;       // Rope advanced successfully
                }
            }
            else
            {
                Main_rope.x--; // Move the rope left
                if (turn)
                {
                    Screen[Main_rope.y][Main_rope.x] = '*'; // Place rope character
                    turn--;                                  // Decrease turn
                }
                else
                {
                    turn++; // Increase turn
                }
                return 1; // Rope advanced successfully
            }
        }
        break;
    case UP:
        if (MC_validshoot(Main_rope.y, Main_rope.x, Main_rope.dir))
        {
            if (Screen[Main_rope.y - 1][Main_rope.x] == '+') // Checks for collision with a cube
            {
                if (MC_bounce90rope()) // Handles 90-degree bounce
                {
                    hitted_cubes++; // Increment hitted cubes count
                    return 1;       // Rope advanced successfully
                }
            }
            else
            {
                Main_rope.y--;            // Move the rope up
                Screen[Main_rope.y][Main_rope.x] = '*'; // Place rope character
                return 1;                  // Rope advanced successfully
            }
        }
    case DOWN:
        if (MC_validshoot(Main_rope.y, Main_rope.x, Main_rope.dir))
        {
            if (Screen[Main_rope.y + 1][Main_rope.x] == '+') // Checks for collision with a cube
            {
                if (MC_bounce90rope()) // Handles 90-degree bounce
                {
                    hitted_cubes++; // Increment hitted cubes count
                    return 1;       // Rope advanced successfully
                }
            }
            else
            {
                Main_rope.y++;            // Move the rope down
                Screen[Main_rope.y][Main_rope.x] = '*'; // Place rope character
                return 1;                  // Rope advanced successfully
            }
        }
    case RIGHT:
        if (MC_validshoot(Main_rope.y, Main_rope.x, Main_rope.dir))
        {
            if (Screen[Main_rope.y][Main_rope.x + 1] == '+') // Checks for collision with a cube
            {
                if (MC_bounce90rope()) // Handles 90-degree bounce
                {
                    hitted_cubes++; // Increment hitted cubes count
                    return 1;       // Rope advanced successfully
                }
            }
            else
            {
                Main_rope.x++; // Move the rope right
                if (turn)
                {
                    Screen[Main_rope.y][Main_rope.x] = '*'; // Place rope character
                    turn--;                                  // Decrease turn
                }
                else
                {
                    turn++; // Increase turn
                }
                return 1; // Rope advanced successfully
            }
        }
        break;
    default:
        return 0; // Unable to advance rope
        break;
    }
    return 0; // Unable to advance rope
}

// Function to shoot the rope
int MC_shoot_rope()
{
    if (!MC_spawn_rope()) // Check if spawning rope is unsuccessful
    {
        return 0; // Unable to shoot rope
    }
    SC_renderscreen(); // Render the screen
    while (MC_advance_rope()) // Continue advancing the rope until it hits an obstacle
    {
        SC_renderscreen(); // Render the screen
        usleep(25000);     // Delay for smooth animation
    }
    return 1; // Rope shot successfully
}

// Function to move the main ghost
void MC_move_ghost(int key)
{
    switch (key)
    {
    case 1: // Move left
        if (Main_Ghost.x - 1 > 0 && MC_Isvalid_move(Main_Ghost.y, Main_Ghost.x, key)) // Check boundary and validity of move
        {
            for (int i = 0; i < 3; i++) // Clear previous ghost position
            {
                Screen[Main_Ghost.y + i][Main_Ghost.x + 5] = ' ';
            }
            Main_Ghost.x--;                       // Move the ghost left
            MC_renderghost(Main_Ghost.y, Main_Ghost.x, 0); // Render the ghost
            Main_Ghost.last_dir_x = LEFT;         // Update last direction
            Main_Ghost.dir = LEFT;                 // Update current direction
        }
        break;
    case 2: // Move up
        if (Main_Ghost.y - 1 > 0 && MC_Isvalid_move(Main_Ghost.y, Main_Ghost.x, key)) // Check boundary and validity of move
        {
            for (int i = 0; i < 6; i++) // Clear previous ghost position
            {
                Screen[Main_Ghost.y + 2][Main_Ghost.x + i] = ' ';
            }
            Main_Ghost.y--;                       // Move the ghost up
            MC_renderghost(Main_Ghost.y, Main_Ghost.x, 1); // Render the ghost
            Main_Ghost.dir = UP;                  // Update current direction
        }
        break;
    case 3: // Move down
        if (Main_Ghost.y + 1 && MC_Isvalid_move(Main_Ghost.y, Main_Ghost.x, key)) // Check boundary and validity of move
        {
            for (int i = 0; i < 6; i++) // Clear previous ghost position
            {
                Screen[Main_Ghost.y][Main_Ghost.x + i] = ' ';
            }
            Main_Ghost.y++;                       // Move the ghost down
            MC_renderghost(Main_Ghost.y, Main_Ghost.x, 2); // Render the ghost
            Main_Ghost.dir = DOWN;                // Update current direction
        }
        break;
    case 4: // Move right
        if (Main_Ghost.x + 1 < SCREEN_WIDE - 7 && MC_Isvalid_move(Main_Ghost.y, Main_Ghost.x, key)) // Check boundary and validity of move
        {
            for (int i = 0; i < 3; i++) // Clear previous ghost position
            {
                Screen[Main_Ghost.y + i][Main_Ghost.x] = ' ';
            }
            Main_Ghost.x++;                       // Move the ghost right
            MC_renderghost(Main_Ghost.y, Main_Ghost.x, 3); // Render the ghost
            Main_Ghost.last_dir_x = RIGHT;        // Update last direction
            Main_Ghost.dir = RIGHT;                // Update current direction
        }
        break;
    case 5: // Shoot a rope
        MC_shoot_rope(); // Shoot the rope
        if (hitted_cubes < num_cubes) // Check if all cubes are hit
        {
            Main_Ghost.num_ropes--; // Decrease rope count
            if (Main_Ghost.num_ropes == 0) // Check if no ropes are left
            {
                SC_Game_over();        // Display game over message
                SC_rendergameover();   // Render game over screen
                while(1){              // Wait for exit command
                    int over_key = KI_read_input();
                    if(over_key == 7){
                        break;
                    }
                }
                exit(0); // Exit the game
            }
            hitted_cubes = 0; // Reset hitted cubes count
        }
        SC_clean_character('*'); // Clean rope character
        clear_stdin_buffer();    // Clear stdin buffer
        break;
    case 6: // Menu option (not implemented)
        snprintf(Lastline_ptr + 67, 80, "chill bro, menu is not implemented still :)"); // Display message
        break;
    case 7: // Exit option
        exit(0); // Exit the game
        break;
    default:
        break;
    }
}


//===============================================CUBE GENERATION FUNCTIONS=================================================================================================

// Function to place a cube on the screen
void put_cube(int y, int x)
{
    for (int i = 0; i < 3; i++) // Iterate over rows of cube ASCII art
    {
        memcpy(Screen[i + y] + x, cube_ascii[i], 6); // Copy cube ASCII characters to screen buffer
    }
}

// Function to check if a cube can be rendered at the specified position
int SG_canrender_cube(int y, int x)
{
    // Check if position is out of bounds
    if (y < 0 || y >= SCREEN_HEIGHT - 5 || x < 0 || x > SCREEN_WIDE - 4)
    {
        return 0; // Cannot render cube
    }
    for (int i = 0; i < 3; i++) // Iterate over rows of cube ASCII art
    {
        for (int j = 0; j < 6; j++) // Iterate over columns of cube ASCII art
        {
            // Check if the position on the screen is already occupied by another object
            if (Screen[i + y][j + x] == '=' || Screen[i + y][j + x] == '|' || Screen[i + y][j + x] == '+' || Screen[i + y][j + x] == 'P')
            {
                return 0; // Cannot render cube
            }
        }
    }
    return 1; // Can render cube
}

// Function to render a cube on the screen
int CG_render_cube(int y, int x, int direction, int last_direction)
{
    // Adjust position based on the direction of movement
    if (last_direction == LEFT)
    {
        x -= 5; // Adjust x-coordinate to the left
        if (direction == DOWN)
        {
            y -= 2; // Adjust y-coordinate downward
        }
    }
    if (last_direction == RIGHT)
    {
        if (direction == DOWN)
        {
            y -= 2; // Adjust y-coordinate downward
        }
    }
    if (last_direction == UP)
    {
        y -= 2; // Adjust y-coordinate upward
        if (direction == RIGHT)
        {
            x -= 5; // Adjust x-coordinate to the left
        }
    }
    if (last_direction == DOWN)
    {
        if (direction == RIGHT)
        {
            x -= 5; // Adjust x-coordinate to the left
        }
    }
    // Check if the cube can be rendered at the specified position
    if (SG_canrender_cube(y, x) == 0)
    {
        return 0; // Cannot render cube
    }
    put_cube(y, x); // Render the cube on the screen
    return 1; // Cube rendered successfully
}


//==================================================================LEVEL GENERATION FUNCTIONS======================================================================================

int GF_generate_random_number() {
    unsigned int rand_num; 

    // Open /dev/urandom to read random bytes
    FILE *urandom_file = fopen("/dev/urandom", "rb"); 
    if (urandom_file != NULL) { 
        // Read a random number from urandom_file
        if (fread(&rand_num, sizeof(rand_num), 1, urandom_file) != 1) { 
            perror("Error reading /dev/urandom"); 
            fclose(urandom_file); 
            exit(EXIT_FAILURE); 
        }
        fclose(urandom_file); 
    } else {
        // If /dev/urandom is not available, use srand with current time as seed
        srand(time(NULL));
        // Generate a random number using rand
        rand_num = rand();
    }

    return rand_num;
}


unsigned int GF_generate_pseudorandom_number(unsigned int seed, unsigned int min, unsigned int max)
{
    // Use srand with the provided seed
    srand(seed);
    // Calculate the range for the random number
    unsigned int range = max - min + 1;
    // Generate a pseudorandom number within the specified range
    return min + rand() % range;
}

void sum1_in_direction(int *y, int *x, int direction)
{
    // Increment or decrement the coordinates based on the direction
    switch (direction)
    {
    case LEFT:
        --(*x);
        break;
    case UP:
        --(*y);
        break;
    case DOWN:
        ++(*y);
        break;
    case RIGHT:
        ++(*x);
        break;
    default:
        // Handle error for invalid direction
        handle_error("error en sum1_in_direction");
        break;
    }
}

void subs1_in_direction(int *y, int *x, int direction)
{
    // Increment or decrement the coordinates based on the direction
    switch (direction)
    {
    case LEFT:
        ++(*x);
        break;
    case UP:
        ++(*y);
        break;
    case DOWN:
        --(*y);
        break;
    case RIGHT:
        --(*x);
        break;
    default:
        // Handle error for invalid direction
        handle_error("error en subs1_in_direction");
        break;
    }
}

int GF_calculate_distance(int y, int x, int direction)
{
    int distance = 0;
    // Move along the specified direction until hitting an obstacle
    while (Screen[y][x] != '|' && Screen[y][x] != '=' && Screen[y][x] != '-' && Screen[y][x] != '+')
    {
        // Handle the special case of portals ('P')
        if (Screen[y][x] == 'P')
        {
            // Adjust coordinates based on the direction of movement
            if (direction == LEFT)
            {
                if ((x - (distance + 7)) >= 7)
                {
                    x -= 6;
                    distance += 6;
                    continue;
                }
                else
                    break;
            }
            else if (direction == RIGHT)
            {
                if ((x + (distance + 6)) <= SCREEN_WIDE - 8)
                {
                    x += 6;
                    distance += 6;
                    continue;
                }
                else
                    break;
            }
            else if (direction == UP)
            {
                if ((y - (distance + 4)) >= 3)
                {
                    y -= 3;
                    distance += 3;
                    continue;
                }
                else
                    break;
            }
            else if (direction == DOWN)
            {
                if ((y + (distance + 3)) <= SCREEN_HEIGHT - 5)
                {
                    y += 3;
                    distance += 3;
                    continue;
                }
                else
                    break;
            }
        }
        // Move one step in the specified direction
        sum1_in_direction(&y, &x, direction);
        distance++;
    }
    // Adjust the coordinates back by one step
    subs1_in_direction(&y, &x, direction);
    distance--;
    return distance;
}

void GF_print_distance(int oldy, int oldx, int newy, int newx)
{
    // Print 'P' characters along a straight line between two points
    if (newx == oldx)
    {
        if (newy > oldy)
        {
            for (int i = oldy + 1; i < newy; i++)
            {
                Screen[i][oldx] = 'P';
            }
        }
        else
        {
            for (int i = newy + 1; i < oldy; i++)
            {
                Screen[i][oldx] = 'P';
            }
        }
    }
    else if (newy == oldy)
    {
        if (newx > oldx)
        {
            for (int i = oldx + 1; i < newx; i++)
            {
                Screen[oldy][i] = 'P';
            }
        }
        else
        {
            for (int i = newx + 1; i < oldx; i++)
            {
                Screen[oldy][i] = 'P';
            }
        }
    }
}

void GF_generate_level(unsigned int random_seed_IN)
{
    int direction, last_direction, distance, old_y, old_x, current_y, current_x, vortex, distance_tries = 0;

    // Initialize the screen with empty spaces
    memcpy(Screen, empty_screen, sizeof(empty_screen));

    // Adjust min and max values for the number of cubes
    if (max_cubes > 20)
    {
        max_cubes = 20;
    }
    if (min_cubes > 15)
    {
        min_cubes = 15;
    }

    // Generate random values for the number of cubes, initial position, and direction
    num_cubes = GF_generate_pseudorandom_number(random_seed_IN, min_cubes, max_cubes);
    last_direction = GF_generate_pseudorandom_number(random_seed_IN, 0, 3);
    old_y = GF_generate_pseudorandom_number(random_seed_IN, 3, SCREEN_HEIGHT - 5);
    old_x = GF_generate_pseudorandom_number(random_seed_IN, 6, SCREEN_WIDE - 8);
    current_y = old_y;
    current_x = old_x;

    // Iterate to generate cubes with randomized positions and distances
    for (int i = 0; i < num_cubes; i++)
    {
        while (distance_tries < 128)
        {
            // Generate a random direction
            direction = GF_generate_pseudorandom_number(random_seed_IN, 0, 3);
            
            // Ensure that cubes are not placed in consecutive directions
            if (direction == UP || direction == DOWN)
            {
                if (last_direction == UP || last_direction == DOWN)
                {
                    random_seed_IN = GF_generate_pseudorandom_number(random_seed_IN, 0, UINT_MAX - 1);
                    continue;
                }
            }
            if (direction == LEFT || direction == RIGHT)
            {
                if (last_direction == LEFT || last_direction == RIGHT)
                {
                    random_seed_IN = GF_generate_pseudorandom_number(random_seed_IN, 0, UINT_MAX - 1);
                    continue;
                }
            }
            
            // Calculate the distance to the nearest obstacle
            distance = GF_calculate_distance(old_y, old_x, direction);
            
            // Handle special cases for direction and distance
            if (direction == UP || direction == DOWN)
            {
                if (distance > 8)
                {
                    // Generate a random vortex within the allowed distance
                    vortex = GF_generate_pseudorandom_number(random_seed_IN, 4, distance - 4);
                    if (direction == UP)
                    {
                        current_y = old_y - vortex;
                        // Render the cube and exit the loop
                        if (CG_render_cube(old_y, old_x, direction, last_direction))
                        {
                            break;
                        }
                        else
                        {
                            distance_tries++;
                            continue;
                        }
                    }
                    else
                    {
                        current_y = old_y + vortex;
                        // Render the cube and exit the loop
                        if (CG_render_cube(old_y, old_x, direction, last_direction))
                        {
                            break;
                        }
                        else
                        {
                            distance_tries++;
                            continue;
                        }
                    }
                }
            }
            else
            {
                if (distance > 15)
                {
                    // Generate a random vortex within the allowed distance
                    vortex = GF_generate_pseudorandom_number(random_seed_IN, 8, distance - 8);
                    if (direction == LEFT)
                    {
                        current_x = old_x - vortex;
                        // Render the cube and exit the loop
                        if (CG_render_cube(old_y, old_x, direction, last_direction))
                        {
                            break;
                        }
                        else
                        {
                            distance_tries++;
                            continue;
                        }
                    }
                    else
                    {
                        current_x = old_x + vortex;
                        // Render the cube and exit the loop
                        if (CG_render_cube(old_y, old_x, direction, last_direction))
                        {
                            break;
                        }
                        else
                        {
                            distance_tries++;
                            continue;
                        }
                    }
                }
            }
            distance_tries++;
            random_seed_IN = GF_generate_pseudorandom_number(random_seed_IN, 0, UINT_MAX - 1);
        }
        // Exit loop if maximum tries exceeded
        if (distance_tries >= 128)
        {
            num_cubes = i;
            break;
        }
        // Print the path between cubes
        if (i != num_cubes - 1)
            GF_print_distance(old_y, old_x, current_y, current_x);

        // Update variables for the next cube
        last_direction = direction;
        old_y = current_y;
        old_x = current_x;
        distance_tries = 0;
    }
    // Clean 'P' characters from the screen
    SC_clean_character('P');
}


//=================================================================MAIN====================================================================================================

int main()
{
    int Key;
    unsigned int seed;
    TR_configure_Terminal(); // setup terminal on non canonical mode and no echo
    // setup initial connfig
    Main_Ghost.num_ropes = 4;
    Main_Ghost.num_levels = 1;

    min_cubes = 6;
    max_cubes = 12;

    // render title screen
    SC_Title_Screen();

    while (1)
    {
        do
        {
            seed = GF_generate_random_number();
            GF_generate_level(seed);
        } while (num_cubes < min_cubes);
        snprintf(Lastline_ptr, SCREEN_WIDE - 2, "Level number: %d | num of ropes available : %d | seed : %u", Main_Ghost.num_levels, Main_Ghost.num_ropes, seed);
        MC_spawn_Ghost(seed);
        SC_renderscreen();
        while (1)
        {
            Key = KI_read_input();
            MC_move_ghost(Key);
            snprintf(Lastline_ptr, SCREEN_WIDE - 2, "Level number: %d | num of ropes available : %d | seed : %u", Main_Ghost.num_levels, Main_Ghost.num_ropes, seed);
            SC_renderscreen();
            usleep(35000);
            if (num_cubes <= hitted_cubes)
            {
                Main_Ghost.num_levels++;
                if (Main_Ghost.num_levels % 3 == 0)
                {
                    min_cubes++;
                    max_cubes++;
                }
                break;
            }
        }
        hitted_cubes = 0;
        num_cubes = 0;
    }
    return 0;
}
