// Gal Snir 313588279

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>

#define TETRIS "./draw.out"
#define TURN 'w'
#define DOWN 's'
#define LEFT 'a'
#define RIGHT 'd'
#define QUIT 'q'

char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror ("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror ("tcsetattr ~ICANON");
    return (buf);
}

// This method print an error message when it occurs
void printError()
{
    char* msg = "Error in system call\n";
    write(2,msg,sizeof(msg));
}

// This method checks if the inserted key is valid
int checkIfKey(char c)
{
    if ((c == RIGHT) ||  (c == LEFT) || (c == TURN) || (c == DOWN) || (c == QUIT))
    {
        return 1;
    }

    else
    {
        return 0;
    }
}


int main() {
    int Pipe [2];
    pipe(Pipe);
    int pid;

    if((pid = fork()) < 0)
    {
        printError();
    }

    //  We create a son process and let him run the tetris
    if (pid == 0)
    {
        dup2(Pipe[0],0);
        if (execlp(TETRIS,TETRIS,NULL) < 0)
        {
            printError();
        }
    }

    // The father process keeps getting keys from the user and sends them to the tetris via signals
    char c;
    while(1)
    {
        c = getch();

        if (!checkIfKey(c))
        {
            continue;
        }

        if (write(Pipe[1],&c,1) < 0)
        {
            printError();
        }


        kill(pid,SIGUSR2);

        if (c == QUIT)
        {
            break;
        }
    }
    return 0;
}
