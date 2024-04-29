#include "CUBE_BASH.h"



//==================================================================0GLOBAL VARIABLES=============================================================================================

ghost Main_Ghost;

rope Main_rope;

static unsigned char Screen[SCREEN_HEIGHT][SCREEN_WIDE]; // matrix where the game is being played

unsigned char *Lastline_ptr = Screen[SCREEN_HEIGHT - 1]; // pointer to write messages on the last line

static struct termios old_termios, new_termios; // for configure terminal on non canoncial mode, and no echo

unsigned int num_cubes = 0;

unsigned int min_cubes = 0;

unsigned int max_cubes = 0;

unsigned int hitted_cubes = 0;

// ==============================================================SCREEN FUNCTIONS============================================================================================================
void SC_moveCursor(unsigned int x, unsigned int y)
{
    if (x >= SCREEN_WIDE || y >= SCREEN_HEIGHT)
    { // comprobación anti-yo
        printf("NOPEEEEEEEEEEEEEEEEEEEEEE\n");
    }
    else
    {
        printf("\033[%d;%dH", x, y);
    }
}

void SC_renderTitle(int render_time)
{
    printf("\033[2J");
    SC_moveCursor(1, 1);
    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < SCREEN_WIDE; j++)
        {
            if (i == 0 || j == 0 || j == SCREEN_WIDE - 2 || i == SCREEN_HEIGHT - 2)
            {
                printf("\033[0;31m");
                putchar(Screen[i][j]);
                printf("\033[0m");
                continue;
            }
            else if (Screen[i][j] == 'O')
            {
                printf("\033[1;32m");
                putchar(Screen[i][j]);
                printf("\033[0m");
                continue;
            }
            else if (Screen[i][j] == '(' || Screen[i][j] == ')')
            {
                printf("\033[1;34m");
                putchar(Screen[i][j]);
                printf("\033[0m");
                continue;
            }
            else if (i < 11)
            {
                printf("\033[1;35m");
                putchar(Screen[i][j]);
                printf("\033[0m");
                continue;
            }
            else if (Screen[i][j] == '-' || Screen[i][j] == '>' || Screen[i][j] == '<')
            {
                printf("\033[1;34m");
                putchar(Screen[i][j]);
                printf("\033[0m");
                continue;
            }

            printf("\033[0;35m");
            putchar(Screen[i][j]);
            printf("\033[0m");
        }
        if (render_time)
        {
            usleep(50000);
        }
    }
    fflush(stdout);
}

void SC_renderscreen()
{
    SC_moveCursor(1, 1);
    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < SCREEN_WIDE; j++)
        {
            if (i == 0 || (j == 0 && i != SCREEN_HEIGHT - 1) || j == SCREEN_WIDE - 2 || i == SCREEN_HEIGHT - 2) //
            {
                printf("\033[0;31m");
                putchar(Screen[i][j]);
                printf("\033[0m");
                continue;
            }
            else if (Screen[i][j] == 'O' || (Screen[i][j] == 'o' && i != SCREEN_HEIGHT - 1))
            {
                printf("\033[1;32m");
                putchar(Screen[i][j]);
                printf("\033[0m");
                continue;
            }
            else if (Screen[i][j] == '(' || Screen[i][j] == ')' || Screen[i][j] == '-' || Screen[i][j] == '.')
            {
                printf("\033[1;34m");
                putchar(Screen[i][j]);
                printf("\033[0m");
                continue;
            }
            else if (Screen[i][j] == 'U' || Screen[i][j] == '\"' || Screen[i][j] == '\'')
            {
                printf("\033[0;35m");
                putchar(Screen[i][j]);
                printf("\033[0m");
                continue;
            }
            else if (Screen[i][j] == '=' || Screen[i][j] == '|')
            {
                printf("\033[1;35m");
                putchar(Screen[i][j]);
                printf("\033[0m");
                continue;
            }
            else if (Screen[i][j] == '+')
            {
                printf("\033[1;33m");
                putchar(Screen[i][j]);
                printf("\033[0m");
                continue;
            }
            else if (Screen[i][j] == '*')
            {
                printf("\033[1;34m");
                putchar(Screen[i][j]);
                printf("\033[0m");
                continue;
            }
            else if (i == SCREEN_HEIGHT - 1)
            {
                printf("\033[1;31m");
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
            break;
        }
        SC_renderTitle(FAST_RENDER);
    }
}

void SC_Game_over()
{
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
    SC_moveCursor(1, 1);
    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < SCREEN_WIDE; j++)
        {
            printf("\033[1;30m"); 
            putchar(Screen[i][j]);
        }
    }
    fflush(stdout);
}

//=================================================TERMINAL FUNCTIONS=========================================================================================

// devuelve la terminal la estado pre-juego
void TR_reset_terminal()
{
    printf("\e[m");    // reset color changes
    printf("\e[?25h"); // enseñar el cursor otra vez
    printf("\033[2J");
    fflush(stdout);
    SC_moveCursor(1, 1);
    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios); // cargamos setup de terminal antiguo
}

// configura la terminal para no echo para que no escriba nada al escribir, y
// pone el non-canocial mode para que lea el input de forma instantanea
void TR_configure_Terminal()
{
    tcgetattr(STDIN_FILENO, &old_termios);
    new_termios = old_termios;

    new_termios.c_lflag &= ~(ICANON | ECHO); // apagamos el echo en terminal, y ponemos en non-canocial mode
    new_termios.c_cc[VMIN] = 0;
    new_termios.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
    printf("\033[?25l"); // esconder el cursor
    atexit(TR_reset_terminal);
}

//===============================================================HANDLE ERROR================================================================================================================

void handle_error(unsigned char *error_message)
{
    TR_reset_terminal();
    printf("%s\n", error_message);
    sleep(5);
    exit(0);
}

//================================================KEY INPUT FUNCTIONS=======================================================================================================

int KI_read_key(char *buff, int k)
{
    char allowed_keys[] = {'a', 'w', 's', 'd', 'p', 'm', '\n'};
    for (int i = 0; i < sizeof(allowed_keys); i++)
    {
        if (buff[k] == allowed_keys[i])
        {
            return i + 1; // Devolver un valor único para cada tecla
        }
    }
    return 0; // No es una tecla válida
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
            break; // Solo necesitamos una tecla válida
        }
    }
    return final_key;
}

void clear_stdin_buffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ; // Leer y descartar caracteres hasta encontrar una nueva línea o el final del archivo
}

//===============================================MAIN CHARACTER FUNCTIONS==========================================================================
unsigned int MC_checkspawn(int y, int x)
{
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

int check_colision_rope(unsigned int y, unsigned int x)
{
    if (Screen[y][x] != ' ' && Screen[y][x] != '+' && Screen[y][x] != '*')
    {
        return 0;
    }
    return 1;
}

int MC_validshoot(unsigned int y, unsigned int x, unsigned int direction)
{
    switch (direction)
    {
    case LEFT:
        return check_colision_rope(y, x - 1);
    case UP:
        return check_colision_rope(y - 1, x);
    case DOWN:
        return check_colision_rope(y + 1, x);
    case RIGHT:
        return check_colision_rope(y, x + 1);
    default:
        return 0;
    }
}
int MC_spawn_rope()
{
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
                if (MC_validshoot(Main_Ghost.y - 2, Main_Ghost.x + 4, DOWN)  && Screen[Main_Ghost.y + 3][Main_Ghost.x + 4] != '+')
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

int MC_bounce90rope()
{
    if (Main_rope.dir == LEFT)
    {
        if (Screen[Main_rope.y + 1][Main_rope.x - 1] == '|')
        {
            if (check_colision_rope(Main_rope.y - 1, Main_rope.x - 1))
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
            if (check_colision_rope(Main_rope.y + 1, Main_rope.x - 1))
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
            if (check_colision_rope(Main_rope.y - 1, Main_rope.x + 1))
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
            if (check_colision_rope(Main_rope.y + 1, Main_rope.x + 1))
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
            if (check_colision_rope(Main_Ghost.y - 1, Main_Ghost.x + 1))
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
            if (check_colision_rope(Main_rope.y - 1, Main_rope.x - 1))
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
            if (check_colision_rope(Main_Ghost.y + 1, Main_Ghost.x + 1))
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
            if (check_colision_rope(Main_rope.y + 1, Main_rope.x - 1))
            {
                Main_rope.y++;
                Main_rope.x--;
                Main_rope.dir = LEFT;
                Screen[Main_rope.y][Main_rope.x] = '*';
                return 1;
            }
        }
    }
    return 0;
}

int MC_advance_rope()
{
    static unsigned int turn = 0;
    switch (Main_rope.dir)
    {
    case LEFT:
        if (MC_validshoot(Main_rope.y, Main_rope.x, Main_rope.dir))
        {
            if (Screen[Main_rope.y][Main_rope.x - 1] == '+')
            {
                if (MC_bounce90rope())
                {
                    hitted_cubes++;
                    return 1;
                }
            }
            else
            {
                Main_rope.x--;
                if (turn)
                {
                    Screen[Main_rope.y][Main_rope.x] = '*';
                    turn--;
                }
                else
                {
                    turn++;
                }
                return 1;
            }
        }
        break;
    case UP:
        if (MC_validshoot(Main_rope.y, Main_rope.x, Main_rope.dir))
        {
            if (Screen[Main_rope.y - 1][Main_rope.x] == '+')
            {
                if (MC_bounce90rope())
                {
                    hitted_cubes++;
                    return 1;
                }
            }
            else
            {
                Main_rope.y--;
                Screen[Main_rope.y][Main_rope.x] = '*';
                return 1;
            }
        }
    case DOWN:
        if (MC_validshoot(Main_rope.y, Main_rope.x, Main_rope.dir))
        {
            if (Screen[Main_rope.y + 1][Main_rope.x] == '+')
            {
                if (MC_bounce90rope())
                {
                    hitted_cubes++;
                    return 1;
                }
            }
            else
            {
                Main_rope.y++;
                Screen[Main_rope.y][Main_rope.x] = '*';
                return 1;
            }
        }
    case RIGHT:
        if (MC_validshoot(Main_rope.y, Main_rope.x, Main_rope.dir))
        {
            if (Screen[Main_rope.y][Main_rope.x + 1] == '+')
            {
                if (MC_bounce90rope())
                {
                    hitted_cubes++;
                    return 1;
                }
            }
            else
            {
                Main_rope.x++;
                if (turn)
                {
                    Screen[Main_rope.y][Main_rope.x] = '*';
                    turn--;
                }
                else
                {
                    turn++;
                }
                return 1;
            }
        }
        break;
    default:
        return 0;
        break;
    }
    return 0;
}

int MC_shoot_rope()
{
    if (!MC_spawn_rope())
    {
        return 0;
    }
    SC_renderscreen();
    while (MC_advance_rope())
    {
        SC_renderscreen();
        usleep(25000);
    }
    return 1;
}

void MC_move_ghost(int key)
{
    switch (key)
    {
    case 1: // Mover a la izquierda
        if (Main_Ghost.x - 1 > 0 && MC_Isvalid_move(Main_Ghost.y, Main_Ghost.x, key))
        { // Ajuste de la condición para el borde izquierdo
            for (int i = 0; i < 3; i++)
            {
                Screen[Main_Ghost.y + i][Main_Ghost.x + 5] = ' ';
            }
            Main_Ghost.x--;
            MC_renderghost(Main_Ghost.y, Main_Ghost.x, 0);
            Main_Ghost.last_dir_x = LEFT;
            Main_Ghost.dir = LEFT;
        }
        break;
    case 2: // Mover hacia arriba
        if (Main_Ghost.y - 1 > 0 && MC_Isvalid_move(Main_Ghost.y, Main_Ghost.x, key))
        { // Ajuste de la condición para el borde superior
            for (int i = 0; i < 6; i++)
            {
                Screen[Main_Ghost.y + 2][Main_Ghost.x + i] = ' ';
            }
            Main_Ghost.y--;
            MC_renderghost(Main_Ghost.y, Main_Ghost.x, 1);
            Main_Ghost.dir = UP;
        }
        break;
    case 3: // Mover hacia abajo
        if (Main_Ghost.y + 1 && MC_Isvalid_move(Main_Ghost.y, Main_Ghost.x, key))
        { // Ajuste de la condición para el borde inferior
            for (int i = 0; i < 6; i++)
            {
                Screen[Main_Ghost.y][Main_Ghost.x + i] = ' ';
            }
            Main_Ghost.y++;
            MC_renderghost(Main_Ghost.y, Main_Ghost.x, 2);
            Main_Ghost.dir = DOWN;
        }
        break;
    case 4: // Mover a la derecha
        if (Main_Ghost.x + 1 < SCREEN_WIDE - 7 && MC_Isvalid_move(Main_Ghost.y, Main_Ghost.x, key))
        { // Ajuste de la condición para el borde derecho
            for (int i = 0; i < 3; i++)
            {
                Screen[Main_Ghost.y + i][Main_Ghost.x] = ' ';
            }
            Main_Ghost.x++;
            MC_renderghost(Main_Ghost.y, Main_Ghost.x, 3);
            Main_Ghost.last_dir_x = RIGHT;
            Main_Ghost.dir = RIGHT;
        }
        break;
    case 5: // shoot a rope
        MC_shoot_rope();
        if (hitted_cubes < num_cubes)
        {
            Main_Ghost.num_ropes--;
            if (Main_Ghost.num_ropes == 0)
            {
                SC_Game_over();
                SC_rendergameover();
                while(1){
                    int over_key = KI_read_input();
                    if(over_key == 7){
                        break;
                    }
                }
                exit(0);
            }
            hitted_cubes = 0;
        }
        SC_clean_character('*');
        clear_stdin_buffer();
        break;
    case 6:
        snprintf(Lastline_ptr + 67, 80, "chill bro, menu is not implemented still :)");
        break;
    case 7:
        exit(0);
        break;
    default:
        break;
    }
}

//===============================================CUBE GENERATION FUNCTIONS=================================================================================================

void put_cube(int y, int x)
{
    for (int i = 0; i < 3; i++)
    {
        memcpy(Screen[i + y] + x, cube_ascii[i], 6);
    }
}

int SG_canrender_cube(int y, int x)
{
    if (y < 0 || y >= SCREEN_HEIGHT - 5 || x < 0 || x > SCREEN_WIDE - 4)
    {
        return 0;
    }
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            if (Screen[i + y][j + x] == '=' || Screen[i + y][j + x] == '|' || Screen[i + y][j + x] == '+' || Screen[i + y][j + x] == 'P')
            {
                return 0;
            }
        }
    }
    return 1;
}

int CG_render_cube(int y, int x, int direction, int last_direction)
{
    if (last_direction == LEFT)
    {
        x -= 5;
        if (direction == DOWN)
        {
            y -= 2;
        }
    }
    if (last_direction == RIGHT)
    {
        if (direction == DOWN)
        {
            y -= 2;
        }
    }
    if (last_direction == UP)
    {
        y -= 2;
        if (direction == RIGHT)
        {
            x -= 5;
        }
    }
    if (last_direction == DOWN)
    {
        if (direction == RIGHT)
        {
            x -= 5;
        }
    }
    if (SG_canrender_cube(y, x) == 0)
    {
        return 0;
    }
    put_cube(y, x);
    return 1;
}
//==================================================================LEVEL GENERATION FUNCTIONS======================================================================================

int GF_generate_random_number() {
    unsigned int rand_num;

    FILE *urandom_file = fopen("/dev/urandom", "rb");
    if (urandom_file != NULL) {
        if (fread(&rand_num, sizeof(rand_num), 1, urandom_file) != 1) {
            perror("Error reading /dev/urandom");
            fclose(urandom_file);
            exit(EXIT_FAILURE);
        }
        fclose(urandom_file);
    } else {
        // If /dev/urandom is not available, use srand with current time as seed
        srand(time(NULL));
        rand_num = rand();
    }

    return rand_num;
}


unsigned int GF_generate_pseudorandom_number(unsigned int seed, unsigned int min, unsigned int max)
{ // pseudonumber generator using srand from stdlib
    srand(seed);
    unsigned int range = max - min + 1;
    return min + rand() % range;
}

void sum1_in_direction(int *y, int *x, int direction)
{
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
        handle_error("error en sum1_in_direction");
        break;
    }
}

void subs1_in_direction(int *y, int *x, int direction)
{
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
        handle_error("error en subs1_in_direction");
        break;
    }
}

int GF_calculate_distance(int y, int x, int direction)
{
    int distance = 0;
    while (Screen[y][x] != '|' && Screen[y][x] != '=' && Screen[y][x] != '-' && Screen[y][x] != '+')
    {
        if (Screen[y][x] == 'P')
        {
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
        sum1_in_direction(&y, &x, direction);
        distance++;
    }
    subs1_in_direction(&y, &x, direction);
    distance--;
    return distance;
}

void GF_print_distance(int oldy, int oldx, int newy, int newx)
{
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

    memcpy(Screen, empty_screen, sizeof(empty_screen));
    if (max_cubes > 20)
    {
        max_cubes = 20;
    }
    if (min_cubes > 15)
    {
        min_cubes = 15;
    }
    // initial setup
    num_cubes = GF_generate_pseudorandom_number(random_seed_IN, min_cubes, max_cubes);
    last_direction = GF_generate_pseudorandom_number(random_seed_IN, 0, 3);
    old_y = GF_generate_pseudorandom_number(random_seed_IN, 3, SCREEN_HEIGHT - 5);
    old_x = GF_generate_pseudorandom_number(random_seed_IN, 6, SCREEN_WIDE - 8);
    current_y = old_y;
    current_x = old_x;

    for (int i = 0; i < num_cubes; i++)
    {
        while (distance_tries < 128)
        {
            direction = GF_generate_pseudorandom_number(random_seed_IN, 0, 3);
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
            distance = GF_calculate_distance(old_y, old_x, direction);
            if (direction == UP || direction == DOWN)
            {
                if (distance > 8)
                {
                    vortex = GF_generate_pseudorandom_number(random_seed_IN, 4, distance - 4);
                    if (direction == UP)
                    {
                        current_y = old_y - vortex;

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
                    vortex = GF_generate_pseudorandom_number(random_seed_IN, 8, distance - 8);
                    if (direction == LEFT)
                    {
                        current_x = old_x - vortex;
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
        if (distance_tries >= 128)
        {
            num_cubes = i;
            break;
        }
        if (i != num_cubes - 1)
            GF_print_distance(old_y, old_x, current_y, current_x);

        last_direction = direction;
        old_y = current_y;
        old_x = current_x;
        distance_tries = 0;
    }
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
