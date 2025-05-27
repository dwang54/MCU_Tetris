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


const uint16_t colors[7] = {
    0x07FF, // I - Cyan
    0xFFE0, // O - Yellow
    0xF81F, // T - Purple
    0x07E0, // S - Green
    0xF800, // Z - Red
    0x001F, // J - Blue
    0xFD20  // L - Orange
};

typedef struct {
    int x, y;                       // Position on grid
    uint8_t shape[4][4];           // 4x4 block matrix
    uint16_t color;                // Color of block
} Tetromino;

int score = 0;

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
        LCD_DrawLine(x, 0, x, LCD_HEIGHT, 0x7BEF); 
    }
}


uint8_t grid[GRID_ROWS][GRID_COLS] = {0};
Tetromino current_piece;

//============================================================================
// Spawns a new random tetromino at top
//============================================================================
void spawn_piece() {
    int index = rand() % 7;
    memset(current_piece.shape, 0, sizeof(current_piece.shape));
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            current_piece.shape[i][j] = tetrominoes[index][i][j];
    current_piece.color = colors[index];
    current_piece.x = 3;
    current_piece.y = 0;

    // Game over check: new piece overlaps existing blocks
    if (!valid_position(current_piece.x, current_piece.y, current_piece.shape)) {
        printf("Game Over!\n");
        while (1); // Halts the game. Alternatively, reset or prompt restart.
    }
}


//============================================================================
// Drawing a single block
//============================================================================
void draw_block(int x, int y, uint16_t color) {
    int x0 = x * BLOCK_SIZE;
    int y0 = y * BLOCK_SIZE;
    LCD_DrawFillRectangle(x0, y0, x0 + BLOCK_SIZE - 1, y0 + BLOCK_SIZE - 1, color);
}

//============================================================================
// Draws the current piece
//============================================================================
void draw_piece() {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (current_piece.shape[i][j])
                draw_block(current_piece.x + j, current_piece.y + i, current_piece.color);
}

//============================================================================
// Erases the current piece
//============================================================================
void erase_piece() {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (current_piece.shape[i][j])
                draw_block(current_piece.x + j, current_piece.y + i, 0x0000);
}
//============================================================================
// Clockwise rotation for tetromino
//============================================================================
void rotate_clockwise(uint8_t mat[4][4], uint8_t out[4][4]) {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            out[j][3 - i] = mat[i][j];
}


//============================================================================
// Counterclockwise rotation for tetromino
//============================================================================
void rotate_counterclockwise(uint8_t mat[4][4], uint8_t out[4][4]) {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            out[3 - j][i] = mat[i][j];
}


//============================================================================
// Check if a position is valid for current piece
//============================================================================
int valid_position(int x, int y, uint8_t shape[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (!shape[i][j]) continue;
            int nx = x + j;
            int ny = y + i;
            if (nx < 0 || nx >= GRID_COLS || ny >= GRID_ROWS) return 0;
            if (grid[ny][nx]) return 0;
        }
    }
    return 1;
}


//============================================================================
// Locks the current piece into the grid
//============================================================================
void lock_piece() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (current_piece.shape[i][j]) {
                int gx = current_piece.x + j;
                int gy = current_piece.y + i;
                if (gx >= 0 && gx < GRID_COLS && gy >= 0 && gy < GRID_ROWS) {
                    grid[gy][gx] = current_piece.color;
                }
            }
        }
    }
}


//============================================================================
// Clears full lines and shifts down
//============================================================================
int clear_lines() {
    int lines_cleared = 0;
    for (int i = GRID_ROWS - 1; i >= 0; i--) {
        int full = 1;
        for (int j = 0; j < GRID_COLS; j++)
            if (!grid[i][j]) {
                full = 0;
                break;
            }
        if (full) {
            lines_cleared++;
            for (int k = i; k > 0; k--)
                for (int j = 0; j < GRID_COLS; j++)
                    grid[k][j] = grid[k - 1][j];
            for (int j = 0; j < GRID_COLS; j++)
                grid[0][j] = 0;
            i++; // recheck same row
        }
    }
    return lines_cleared;
}


//============================================================================
// Moves current piece down, spawns new if it can’t move
//============================================================================
void drop_piece() {
    erase_piece();
    if (valid_position(current_piece.x, current_piece.y + 1, current_piece.shape)) {
        current_piece.y++;
    } else {
        lock_piece();
        int lines = clear_lines();
        if (lines > 0) {
            score += lines * POINTS;
            printf("Score: %d\n", score);  // Optional debug output
        }
        spawn_piece();
    }
    draw_piece();
}


//============================================================================
// Handles piece movement left/right/rotation
//============================================================================
void move_left() {
    erase_piece();
    if (valid_position(current_piece.x - 1, current_piece.y, current_piece.shape))
        current_piece.x--;
    draw_piece();
}

void move_right() {
    erase_piece();
    if (valid_position(current_piece.x + 1, current_piece.y, current_piece.shape))
        current_piece.x++;
    draw_piece();
}

void rotate_current(int clockwise) {
    uint8_t rotated[4][4];
    if (clockwise)
        rotate_clockwise(current_piece.shape, rotated);
    else
        rotate_counterclockwise(current_piece.shape, rotated);

    erase_piece();
    if (valid_position(current_piece.x, current_piece.y, rotated))
        memcpy(current_piece.shape, rotated, sizeof(rotated));
    draw_piece();
}

void draw_grid_state() {
    for (int i = 0; i < GRID_ROWS; i++) {
        for (int j = 0; j < GRID_COLS; j++) {
            draw_block(j, i, grid[i][j]);
        }
    }
}


void game_loop() {
    init_game();
    spawn_piece();
    draw_piece();

    while (1) {
        // poll input: move_left(), move_right(), rotate_current()
        // optional: drop fast if key held

        delay_ms(500); // control speed
        drop_piece();  // advance one row

        draw_grid_state();  // refresh background grid
    }
}







