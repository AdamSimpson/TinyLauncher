#ifndef LAUNCHER_H
#define LAUNCHER_H

typedef struct launcher_t {

    int cursor_x;
    int cursor_y;

} launcher_t;

void update_cursor(launcher_t *state, int x, int y);

#endif
