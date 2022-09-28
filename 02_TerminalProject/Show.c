#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DX 3
#define FREE_LINES LINES - 2 * DX - 2
#define FREE_COLS COLS - 2 * DX - 2


void get_text(FILE *f, char ***text, size_t *len_num, size_t **line_lens) {
    
    char *line = NULL;
    size_t n = 0;
    size_t current_num = 0;

    *text = NULL;
    *line_lens = NULL;

    size_t len ;
    while ((len = getline(&line, &n, f)) != -1) {

        // delete '\n'

        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        current_num += 1;

        // increase array of pointers to strings
        *text = realloc(*text, current_num * sizeof(char*));
        // increase array of string lengths
        *line_lens = realloc(*line_lens, current_num * sizeof(size_t));

        // add pointer of new string and length
        (*text)[current_num - 1] = line;
        (*line_lens)[current_num - 1] = strlen(line);
        
        line = NULL;
    }

    *len_num = current_num;
    free(line);
}


void clear_memory(char **text, size_t *line_lens, const size_t num_of_lines) {
    size_t i;
    for (i = 0; i < num_of_lines; ++i) {
          free(text[i]);
    }
    free(text);
    free(line_lens);
}


void update_window(WINDOW *win, const int start_line, const int start_col,
                   char **text, const size_t *line_lens, size_t num_of_lines) {

    werase(win);
    wmove(win, 1, 0);
    
    int i;
    for (i = 0; (i < num_of_lines - start_line) && (i < FREE_LINES); ++i) {
        wprintw(win, " %3d: ", i + start_line);
        int j;
        if (start_col < line_lens[i + start_line]) {
            for (j = 0; j < line_lens[i + start_line] - start_col && j < FREE_COLS - 5; ++j) {
                waddch(win, text[i + start_line][j + start_col]);
            }
        }
        waddch(win, '\n');
    }

    box(win, 0, 0);
    wmove(win, 1, 3); 
    wrefresh(win);
}



int main(int argc, char *argv[]) {

    // check data and open file

    if (argc != 2) {
        printf("Invalid arguments\nUsage example: %s your_file.txt\n", argv[0]);
        return 1;
    }

    FILE* f = fopen(argv[1], "r");

    if (f == NULL) {
        printf("Unable to open file: %s\n", argv[1]);
        return 2;
    }

    
    // read file

    char **text;
    size_t *line_lens;
    size_t num_of_lines;

    get_text(f, &text, &num_of_lines, &line_lens);

    fclose(f);

    // setups and window init

    initscr();
    noecho();
    cbreak();

    move(1, 0);
    printw("%s, %d lines:", argv[1], num_of_lines);
    refresh();

    WINDOW *win = newwin(FREE_LINES + 2, FREE_COLS + 2, DX, DX);

    keypad(win, true);


    // main loop with handlers

    int start_line = 0, start_col = 0;
    bool flag = true;

    while (flag) {
        update_window(win, start_line, start_col, text, line_lens, num_of_lines);

        int c = wgetch(win);

        switch (c) {
            case ' ':
            case KEY_DOWN:
                if (start_line + 1 < num_of_lines) {
                    start_line += 1;
                }
                break;

            case KEY_NPAGE:
                if (start_line + FREE_LINES < num_of_lines) {
                    start_line += FREE_LINES;
                } else {
                    start_line = num_of_lines - 1;
                }
                break;

            case KEY_UP:
                if (start_line > 0) {
                    start_line -= 1;
                }
                break;

            case KEY_PPAGE:
                if (start_line - FREE_LINES < 0) {
                    start_line = 0;
                } else {
                    start_line -= FREE_LINES;
                }
                break;

            case KEY_LEFT:
                if (start_col > 0) {
                    start_col -= 1;
                }
                break;

            case KEY_RIGHT:
                start_col += 1;
                break;


            // resize the terminal with mouse
            // sometimes it works ugly
            case KEY_RESIZE:
                clear();
                wresize(win, FREE_LINES + 2, FREE_COLS + 2);
                break;

            case 27:  // ESC
                flag = false;
                break;

            default:
                break;
        }
    }


    delwin(win);
    endwin();

    // free memory
    clear_memory(text, line_lens, num_of_lines);
}