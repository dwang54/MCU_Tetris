#include <stdio.h>
#include "defines.h"


#define LCD_WIDTH  240
#define LCD_HEIGHT 320
#define GRID_COLS 10
#define GRID_ROWS 20
#define BLOCK_WIDTH  (LCD_WIDTH / GRID_COLS)   // 24
#define BLOCK_HEIGHT (LCD_HEIGHT / GRID_ROWS)  // 16
#define GRID_X_OFFSET 0
#define GRID_Y_OFFSET 0
#define POINTS 10                               // 10 points for each level cleared
#define BLOCK_SIZE 10






void init_game();
void draw_gridlines();
void game_over();
void spawn_tetrominoes();       // "spawns/draws shape"
void rotate_tetrominoes();
void clear_lines();
void move_piece(int x, int y);



//============================================================================
// Tetromino bitmaps
//============================================================================
uint8_t tetrominoes[7][4][4] = {
    // I
    {
        {0,0,0,0},
        {1,1,1,1},
        {0,0,0,0},
        {0,0,0,0}
    },
    // O
    {
        {0,1,1,0},
        {0,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // T
    {
        {0,1,0,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // S
    {
        {0,1,1,0},
        {1,1,0,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // Z
    {
        {1,1,0,0},
        {0,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // J
    {
        {1,0,0,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // L
    {
        {0,0,1,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    }
};



//============================================================================
// Initializes game
//============================================================================
void init_game() {
    LCD_DrawFillRectangle(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, 0x0000); // black
    draw_gridlines();




}

//============================================================================
// Draws gridlines
//============================================================================
void draw_gridlines() {
    for (int i = 0; i <= GRID_ROWS; ++i) {
        int y = i * BLOCK_HEIGHT;
        LCD_DrawLine(0, y, LCD_WIDTH, y, 0x7BEF); 
    }

    for (int j = 0; j <= GRID_COLS; ++j) {
        int x = j * BLOCK_WIDTH;
        LCD_DrawLine(x, 0, x, LCD_HEIGHT, 0x7BEF); // gray vertical
    }
}


void rotate_clockwise(uint8_t mat[4][4], uint8_t out[4][4]) {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            out[j][3 - i] = mat[i][j];
}


void rotate_counterclockwise(uint8_t mat[4][4], uint8_t out[4][4]) {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            out[3 - j][i] = mat[i][j];
}



void draw_block(int x, int y, uint16_t color) {
    int x0 = x * BLOCK_SIZE;
    int y0 = y * BLOCK_SIZE;
    LCD_DrawFillRectangle(x0, y0, x0 + BLOCK_SIZE - 1, y0 + BLOCK_SIZE - 1, color);
}




