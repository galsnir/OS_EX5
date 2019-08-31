// Gal Snir 313588279

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>

enum direction {HORIZONTAL,VERTICAL};
#define SIZE 20
#define TURN 'w'
#define DOWN 's'
#define LEFT 'a'
#define RIGHT 'd'
#define QUIT 'q'

// This structs hold the information reelvant to the tetris game
typedef struct GAME
{
    char board[SIZE][SIZE];
    int x;
    int y;
    enum direction dir;
} GAME;

GAME game;

// This method calls the print method after a change has been made
void displayChange(GAME* game, int x, int y,int isChanged);

// This method erases the board
void emptyBoard(GAME* game) {
    int i, j;
    for (i = 0; i < SIZE - 1; i++)
    {
        for (j = 0; j < SIZE ; j++)
        {
            if (j == 0 || j == SIZE - 1)
            {
                game->board[i][j] = '*';
            }

            else
            {
                game->board[i][j] = ' ';
            }
        }
    }

    for(j = 0; j < SIZE; j++)
    {
        game->board[SIZE - 1][j] = '*';
    }
}

// This method initilaize the board
void initBoard(GAME* game)
{
    game->dir = HORIZONTAL;
    game->x = 0;
    game->y = SIZE/2 - 1;
    emptyBoard(game);
    displayChange(game,game->x,game->y,0);
}

// This method deletes the shape from the board
void deleteShape (GAME* game)
{
    game->board[game->x][game->y] = ' ';

    if (game->dir == HORIZONTAL)
    {
        game->board[game->x][game->y+1] = ' ';
        game->board[game->x][game->y-1] = ' ';
    }

    else
    {
        game->board[game->x+1][game->y] = ' ';
        game->board[game->x-1][game->y] = ' ';
    }
}

// This method calls the print method after a change has been made
void displayChange(GAME* game, int x, int y,int isChanged)
{
    if(game->dir == HORIZONTAL)
    {
        if (y <= 1 || y >= SIZE-2)
            return;
    }

    if(game->dir == VERTICAL)
    {
        if (y <= 0 || y >= SIZE-1)
            return;
    }

    if (!isChanged)
    {
        deleteShape(game);
    }

    game->x = x;
    game->y = y;

    if(game->dir == HORIZONTAL) {
        game->board[x][y] = '-';
        game->board[x][y + 1] = '-';
        game->board[x][y - 1] = '-';

        if (x >= SIZE-1)
        {
            initBoard(game);
        }

    }

    else {
        game->board[x][y] = '-';
        game->board[x + 1][y] = '-';
        game->board[x - 1][y] = '-';

        if (x >= SIZE-2)
        {
            initBoard(game);
        }

    }
}

// This method changes the direction of the shape on the board
void changeDirection(GAME* game)
{
    int x,y;
    x = game->x;
    y = game->y;
    emptyBoard(game);
    if (game->dir == HORIZONTAL) {
        game->dir = VERTICAL;
        displayChange(game, x, y, 1);
    }

    else
    {
        if (y >= SIZE -2)
            y = SIZE - 4;
        else if (y < 1)
            y = 1;

        game->dir = HORIZONTAL;
        displayChange(game, x, y, 1);
    }
}

// This method erases the screen from the last board and prints the new one with the relevant changes
void printBoard(GAME* game) {
    system("clear");
    int i, j;
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            printf("%c", game->board[i][j]);
        }
        printf("\n");
    }
}

// This method call the displayChange method according to the key she gets through the signal
void getKey(int sign)
{
    signal(SIGUSR2,getKey);

    char c = '\0';
    read(0,&c,1);
    switch (c)
    {
        case RIGHT:
            displayChange(&game, game.x, game.y+1, 0);
            break;
        case LEFT:
            displayChange(&game, game.x, game.y-1, 0);
            break;
        case DOWN:
            displayChange(&game, game.x+1, game.y, 0);
            break;
        case TURN:
            changeDirection(&game);
            break;
        case QUIT:
            exit(1);
        default:
            break;
    }
    printBoard(&game);
}

// This method makes the shape drop one step down in every second
void moveBlock(int sign)
{
    signal(SIGALRM,moveBlock);
    alarm(1);
    displayChange(&game,game.x + 1,game.y,0);
    printBoard(&game);
}

// In the main method we init the board print it and setting the relevant signal for the tetris game
int main()
{
    initBoard(&game);
    printBoard(&game);
    signal(SIGALRM, moveBlock);
    alarm(1);
    signal(SIGUSR2, getKey);

    while (1)
    {
        pause();
    }
}
