#ifndef __UI_UI_H
#define __UI_UI_H

#include <ncurses.h>

#define UIMoveUICursorLeft(count) do {\
    g_cursor->x -= count;\
    if (g_cursor->x < 0) {\
        UIMoveCurMapX(g_cursor->x);\
        g_cursor->x = 0;\
    }\
    wmove(g_rootUIWin->window, g_cursor->y, g_cursor->x);\
} while(0);

#define UIMoveUICursorRight(count) do {\
    g_cursor->x += count;\
    if (g_cursor->x > g_rootUIWin->width) {\
        UIMoveCurMapX(g_cursor->x - g_rootUIWin->width);\
        g_cursor->x = g_rootUIWin->width;\
    }\
    wmove(g_rootUIWin->window, g_cursor->y, g_cursor->x);\
} while(0);

#define UIMoveUICursorUp(count) do {\
    g_cursor->y -= count;\
    if (g_cursor->y < 0) {\
        UIMoveCurMapY(g_cursor->y);\
        g_cursor->y = 0;\
    }\
    wmove(g_rootUIWin->window, g_cursor->y, g_cursor->x);\
} while(0);

#define UIMoveUICursorDown(count) do {\
    g_cursor->y += count;\
    if (g_cursor->y > g_rootUIWin->height) {\
        UIMoveCurMapY(g_cursor->y - g_rootUIWin->height);\
        g_cursor->y = g_rootUIWin->height;\
    }\
    wmove(g_rootUIWin->window, g_cursor->y, g_cursor->x);\
} while(0);

typedef struct {
    int x;
    int y;
    int number;
    char snumber[8];     /* 已输入的数字 */
    int snumber_len;
} UICursor;

enum UIWinMode {gamer_mode, pick_mode};

typedef struct {
    int height;         /* 行数 */
    int width;            /* 列数 */
    int startx;
    int starty;
    int ch;
    enum UIWinMode mode;
    WINDOW *window;
} UIWin;

void UIInit();

#endif
