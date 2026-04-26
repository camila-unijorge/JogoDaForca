#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>

#define MAX_WORD_SIZE 20
struct termios oldt, newt;

#define COLOR_RED 41
#define COLOR_YELLOW 103
#define COLOR_GREEN 42
#define COLOR_DEFAULT 0

#define bool int
#define true 1
#define false 0

void setup_terminal(){
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}
void resestore_terminal(){
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void set_cursor_pos(int collumn, int line) {
    printf("\033[%d;%dH", line+1, collumn+1);
}
void set_style(int color) {
    printf("\033[%dm", color);
}
void clear_screen() {
    puts("\033[2J\033[1;1H");
    fflush(stdout);
}

bool is_letter(char c) {
    return c >= 65 && c <= 90;
}
bool is_backspace(char c) {
    return c == 127 || c == '\b';
}

int game_option = 0;
// 0 = unset
// 1 = you vs computer
// 2 = you vs friend

char word[MAX_WORD_SIZE+1];
int word_len;
int attempt;

void read_word_from_file() {
    FILE *file = fopen("words.txt", "r");
    if (!file) {
        perror("Error opening the file: ");
        exit(1);
    }

    int line_count = 0;
    while (fgets(word, MAX_WORD_SIZE, file)) line_count++;

    if (line_count == 0) {
        printf("words.txt is empty, cannot choose a word!\n");
        fclose(file);
        exit(1);
    }

    srand(time(NULL));
    int random_line = rand() % line_count;
    rewind(file);

    int c = 0;
    while (fgets(word, MAX_WORD_SIZE, file)) {
        if (c == random_line) break;
        c++;
    }

    // Esse loop itera sobre a string lida,
    // substituindo as letras por letras em caixa-alta,
    // fechando a string e parando em LF ou \0
    int i = 0;
    loop_begin: switch (word[i]) {
        case '\n':
        case '\0':
            word[i] = '\0';
            break;
        default:
            word[i++] = toupper(word[i]);
            goto loop_begin;
    }
    
    fclose(file);
}
void read_word_from_user() {
    int i = 0;
    
    set_cursor_pos(0, 0);
    printf("Word: ____________________   0/20");
    fflush(stdout);

    while (1) {
        set_cursor_pos(6+i, 0);
        char c = toupper(getchar());

        if (c == '\n') break;
        
        if (is_backspace(c) && i > 0) {
            putchar('\b');
            putchar('_');
            putchar('\b');
        }
        else if (is_letter && i < MAX_WORD_SIZE) {
            putchar('*');
            word[i++] = c;
        }

        set_cursor_pos(27, 0);
        printf("%3d", i);
        fflush(stdout);
    }

    word[i] = '\0';
}

void reset_game() {
    attempt = 0;
    
    switch (game_option) {
        case 1: read_word_from_file(); break;
        case 2: read_word_from_user(); break;
    }
    clear_screen();
    word_len = strlen(word);
    
    set_cursor_pos(0, 0);
    printf("  ╤═══╦═\n");
    printf("      ║\n");
    printf("      ║\n");
    printf("      ║\n");
    printf("      ║\n");
    printf(" ▒▒▒▒▒▒▒▒\n");
    
    for (int i = 0; i < 6; ++i) {
        set_cursor_pos(1, 7+i);
        for (int j = 0; j < word_len; j++) putchar('_');
    }
    fflush(stdout);
}

void draw_body_part() {
    switch (attempt) {
        case 0: set_cursor_pos(2, 1); putchar('O'); break;
        case 1: set_cursor_pos(2, 2); putchar('|'); break;
        case 2: set_cursor_pos(1, 2); putchar('/'); break;
        case 3: set_cursor_pos(3, 2); putchar('\\'); break;
        case 4: set_cursor_pos(1, 3); putchar('/'); break;
        case 5: set_cursor_pos(3, 3); putchar('\\'); break;
        default: break;
    }
}
bool validate_line(char *word2) {
    bool is_valid = true;
    
    for (int i = 0; i < word_len; ++i) {
        char c = word2[i];
        if (word[i] == c) {
            set_style(COLOR_GREEN);
        }
        else if (strchr(word, c)) {
            is_valid = false;
            set_style(COLOR_YELLOW);
        }
        else {
            is_valid = false;
            set_style(COLOR_RED);
        }
        putchar(c);
    }
    set_style(COLOR_DEFAULT);
    return is_valid;
}

void game_loop() {
    char word2[MAX_WORD_SIZE+1];
    int i = 0;
    
    while (attempt < 6) {
        int line = 7 + attempt;
        set_cursor_pos(1, line);
        
        while (i < word_len) {
            char c = toupper(getchar());
            
            if (is_backspace(c) && i > 0) {
                set_cursor_pos(i+1, line);
                putchar('\b');
                putchar('_');
                putchar('\b');
                --i;
            }
            else if (is_letter(c)) {
                set_cursor_pos(i+1, line);
                putchar(c);
                word2[i++] = c;
            }
        }
        
        set_cursor_pos(1, line);
        if (validate_line(word2)) {
            
            set_cursor_pos(0, 14);
            set_style(COLOR_GREEN);
            printf("You won!\n");
            set_style(COLOR_DEFAULT);
            printf("%d/6 attempts!", attempt+1);
            
            return;
        }
        
        putchar('\a');
        draw_body_part();
        attempt++;
        i = 0;
    }
    
    set_cursor_pos(0, 14);
    set_style(COLOR_RED);
    printf("Hanged!\n");
    set_style(COLOR_DEFAULT);
    printf("The word was '%s'\n", word);
}

int main() {
    setup_terminal();
    
    while (true) {
        clear_screen();
        printf("Select a game option:\n");
        printf("\t1 - you vs computer\n");
        printf("\t2 - you vs friend\n");
        printf("\t3 - quit\n");
        
        do {
            char option = getchar() - '0';
            switch (option) {
                case 1: game_option = 1; break;
                case 2: game_option = 2; break;
                case 3:
                    clear_screen();
                    return 0;
                    break;
                default:
                    printf("\033[41m%c is an invalid option.\033[0m\n", '0' + option);
                    continue;
            }
            break;
        }
        while(true);
    
        clear_screen();
        reset_game();
        game_loop();
        
        printf("\nPress any key to continue");
        getchar();
    }
    return 0;
}