#include <curses.h>
#include <unistd.h>
#include "console_gui.h"

struct Console {
    int width;
    int height;
    int x;
    int y;
};


bool console_init(struct Console **c) {
    if (c == 0) {
        return false;
    }
    (*c) = calloc(1, sizeof(struct Console));
    initscr();
    noecho();
    curs_set(FALSE);

    (*c)->width = getmaxx(stdscr);
    (*c)->height = getmaxy(stdscr);

    (*c)->x = 0;
    (*c)->y = 0;
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    return true;
}

bool console_destroy(struct Console **c) {
    if (c != 0) {
        if ((*c) != 0) {
            endwin();
            free((*c));
        }
        (*c) = 0;
    }
    return true;
}

void console_update(struct Console *c) {
    assert(c != 0);
    refresh();
}

void console_clear(struct Console *c) {
    assert(c != 0);
    clear();
}

enum Key console_translate_key(int key) {
    switch (key) {
        case KEY_UP:
            return KeyUp;
        case KEY_DOWN:
            return KeyDown;
        case KEY_LEFT:
            return KeyLeft;
        case KEY_RIGHT:
            return KeyRight;
        default:
            return KeyUnknown;
    }
}

void console_down(struct Console *c) {
    assert(c != 0);
    ++c->x;
    if (c->x >= console_height(c) - 1)
        c->x = console_height(c) - 1;
    move(c->x, c->y);
}

void console_up(struct Console *c) {
    assert(c != 0);
    --c->x;
    if (c->x < 0)
        c->x = 0;
    move(c->x, c->y);
}

void console_left(struct Console *c) {
    assert(c != 0);
    --c->y;
    if (c->y < 0)
        c->y = 0;
    move(c->x, c->y);
}

void console_right(struct Console *c) {
    assert(c != 0);
    ++c->y;
    if (c->y >= console_width(c) - 1)
        c->y = console_width(c) - 1;
    move(c->x, c->y);
}


int console_height(struct Console *c) {
    assert(c != 0);
    return c->height;
}

int console_width(struct Console *c) {
    assert(c != 0);
    return c->width;
}

void console_text_position(struct Console *c, int x, int y, const char *const text) {
    assert(c != 0);
    mvprintw(x, y, text);
}

void console_text(struct Console *c, const char *const text) {
    DWORD written;

    assert(c != 0);
    mvprintw(c->x, c->y, text);
}

void console_move(struct Console *c, int x, int y) {
    assert(c != 0);
    wmove(stdscr, x, y);
    c->x = x;
    c->y = y;
}

struct Event console_event(struct Console *c) {
    INPUT_RECORD record;
    DWORD read = 0;

    assert(c != 0);
    if (ReadConsoleInput(c->h_input, &record, 1, &read)) {
        if (read == 1) {
            switch (record.EventType) {
                case KEY_EVENT: {
                    if (record.Event.KeyEvent.bKeyDown) {
                        if (record.Event.KeyEvent.uChar.UnicodeChar == 0) {
                            c->ch = record.Event.KeyEvent.wVirtualKeyCode;
                        } else {
                            c->ch = record.Event.KeyEvent.uChar.UnicodeChar;
                        }
                        return KEY_DOWN;
                    } else {
                        return KEY_UP;
                    }
                }
                case WINDOW_BUFFER_SIZE_EVENT: {
                    c->width = record.Event.WindowBufferSizeEvent.dwSize.X;
                    c->height = record.Event.WindowBufferSizeEvent.dwSize.Y;
                    return WINDOW;
                }
                default:
                    break;
            }
        }
    }
    return NONE;
}

int console_ch(struct Console *c) {
    assert(c != 0);
    return wgetch(stdscr);
}
