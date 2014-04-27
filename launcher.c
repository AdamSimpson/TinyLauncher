#include <stdio.h>
#include "egl_utils.h"
#include "image_gl.h"
#include "cursor_gl.h"

int main(int argc, char *argv[])
{

    // Setup initial OpenGL state
    gl_t gl_state;
    memset(&gl_state, 0, sizeof(gl_t));
    init_ogl(&gl_state);

    // Initialize program launcher images OpenGL state
    int image_width = 600;
    int image_height = 375;
    int lower_left_y = gl_state.screen_height/2 - image_height/2;
    int lower_left_x = 250;
    image_t mandelbrot_state;
    init_image(&mandelbrot_state, &gl_state, "TinyLauncher/mandelbrot.png", lower_left_x, lower_left_y, image_width, image_height);
    lower_left_x = gl_state.screen_width - image_width - 250;
    image_t sph_state;
    init_image(&sph_state, &gl_state, "TinyLauncher/sph.png", lower_left_x, lower_left_y, image_width, image_height);

    // Initialize cursor
    cursor_t cursor_state;
    int cursor_width = 30;
    int cursor_height = 40;
    init_cursor(&cursor_state, &gl_state, "TinyLauncher/cursor.png", cursor_width, cursor_height);

    // Set background color
    glClearColor(1.0, 1.0, 1.0, 1.0);

    while(1)
    {
        // Update user input
        check_user_input(&gl_state);

        // Clear background
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw mandelbrot image
        draw_image(&mandelbrot_state);

        // Draw SPH image
        draw_image(&sph_state);

        // Draw cursor
        draw_cursor(&cursor_state);

        // Swap front/back buffers
        swap_ogl(&gl_state);

    }

    return 0;
}
