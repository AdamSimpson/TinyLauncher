#ifndef LAUNCHER_H
#define LAUNCHER_H

typedef struct LAUNCHER_T launcher_t;

#include "cursor_gl.h"
#include "image_gl.h"

struct LAUNCHER_T {
    image_t *mandelbrot_state;
    image_t *sph_state;
    cursor_t *cursor_state;
};

void update_cursor(launcher_t *state, int x, int y);
void check_cursor_in_image(cursor_t *cursor_state, image_t *image_state);
void launch_sph(launcher_t *state);
void launch_mandelbrot(launcher_t *state);
#endif
