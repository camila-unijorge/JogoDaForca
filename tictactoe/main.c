#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#define true 1
#define false 0

struct winsize w = {0};
int scx = 0, scy = 0;

int player1 = 0;
int player2 = 0;
int game_round = 0;
int invalid_input = false;

void predraw() {
    scx = w.ws_col >> 1;
    scy = w.ws_row >> 1;

    printf("\e[%d;%dH 7 | 8 | 9 ", scy-2, scx-5);
    printf("\e[%d;%dH---+---+---", scy-1, scx-5);
    printf("\e[%d;%dH 4 | 5 | 6 ", scy+0, scx-5);
    printf("\e[%d;%dH---+---+---", scy+1, scx-5);
    printf("\e[%d;%dH 1 | 2 | 3 \n", scy+2, scx-5);

}

void prompt() {

    printf("\e[%d;0H\e[2K",scy+4);
    printf("\e[%d;0H\e[2K",scy+5);

    if (invalid_input) printf("\e[%d;%dH\e[31mInput inválido! digite de 1 à 9!\e[0m",scy+4, scx-16);
    invalid_input = false;

    if ((game_round & 1) == 0)
        printf("\e[%d;%dHPlayer 1 digite a posição: ", scy+5, scx-14);
    else
        printf("\e[%d;%dHPlayer 2 digite a posição: ", scy+5, scx-14);

    char input;
    scanf("%c", &input);

    printf("\e[0;0H%c\n", input);
    if (input < '1' || input > '9') {
        invalid_input = true;
        return;
    }
}

void main() {
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    predraw();

    while (1)
    {
        prompt();
    }
    
}
