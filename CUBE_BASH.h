#ifndef CUBEBASH_H
#define CUBEBASH_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <limits.h>

#include "draws.h"
#include "CUBE_BASH.h"
// constants

//constants for title render
#define SLOW_RENDER 1
#define FAST_RENDER 0

//constants for entity movements
#define LEFT 0
#define UP 1
#define DOWN 2
#define RIGHT 3

typedef struct Ghost
{
    unsigned int y;
    unsigned int x;
    unsigned char dir;
    unsigned char last_dir_x;
    unsigned char ghost_sprite[3][6];
    unsigned int num_ropes;
    unsigned int num_levels;
} ghost;

typedef struct Rope
{
    unsigned int y;
    unsigned int x;
    unsigned char dir;
} rope;


// ==============================================================SCREEN FUNCTIONS============================================================================================================

void SC_moveCursor(unsigned int x, unsigned int y);
void SC_renderTitle(int render_time);
void SC_renderscreen();
void SC_clean_character(unsigned char char_in);
void SC_Title_Screen();
void SC_Game_over();
void SC_rendergameover();

//=================================================TERMINAL FUNCTIONS=========================================================================================

void TR_reset_terminal();
void TR_configure_Terminal();

//===============================================================HANDLE ERROR================================================================================================================

void handle_error(unsigned char *error_message);

//================================================KEY INPUT FUNCTIONS=======================================================================================================

int KI_read_key(char *buff, int k);
int KI_read_input();
void clear_stdin_buffer();

//===============================================MAIN CHARACTER FUNCTIONS==========================================================================
unsigned int MC_checkspawn(int y, int x);
void MC_renderghost(int y, int x, int dir);
void MC_spawn_Ghost(unsigned int seed);
int MC_Isvalid_move(int y, int x, int move);
int check_colision_rope(unsigned int y, unsigned int x);
int MC_validshoot(unsigned int y, unsigned int x, unsigned int direction);
int MC_spawn_rope();
int MC_bounce90rope();
int MC_advance_rope();
int MC_shoot_rope();
void MC_move_ghost(int key);

//===============================================CUBE GENERATION FUNCTIONS=================================================================================================

void put_cube(int y, int x);
int SG_canrender_cube(int y, int x);
int CG_render_cube(int y, int x, int direction, int last_direction);

//==================================================================LEVEL GENERATION FUNCTIONS======================================================================================

int GF_generate_random_number();
unsigned int GF_generate_pseudorandom_number(unsigned int seed, unsigned int min, unsigned int max);
void sum1_in_direction(int *y, int *x, int direction);
void subs1_in_direction(int *y, int *x, int direction);
int GF_calculate_distance(int y, int x, int direction);
void GF_print_distance(int oldy, int oldx, int newy, int newx);
void GF_generate_level(unsigned int random_seed_IN);
void SC_Title_Screen();

//   
#endif