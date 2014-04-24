#ifndef LAUNCHER_H
#define LAUNCHER_H

typedef struct LAUNCHER_T launcher_t;

#include "cursor_gl.h"
#include "image_gl.h"

struct LAUNCHER_T {

    int cursor_x;
    int cursor_y;

};

void update_cursor(launcher_t *state, int x, int y);
void check_cursor_in_image(cursor_t *cursor_state, image_t *image_state);

#endif
