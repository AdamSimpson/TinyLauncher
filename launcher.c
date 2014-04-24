#include <stdio.h>
#include "launcher.h"
#include "egl_utils.h"
#include "image_gl.h"
#include "cursor_gl.h"

int main(int argc, char *argv[])
{
    // Create state for launcher program
    launcher_t state;
    // Set cursor to around center of screen initially
    state.cursor_x = 1920/2.0;
    state.cursor_y = 1080/2.0;

    // Setup initial OpenGL state
    gl_t gl_state;
    memset(&gl_state, 0, sizeof(gl_t));
    init_ogl(&gl_state, &state);

    // Initialize program launcher images OpenGL state
    int image_width = 600;
    int image_height = 375;
    int lower_left_y = gl_state.screen_height/2 - image_height/2;
    int lower_left_x = 250;
    image_t mandelbrot_state;
    init_image(&mandelbrot_state, 
               &gl_state, 
               "TinyLauncher/mandelbrot.png",
               "TinyLauncher/mandelbrot-selected.png",
               lower_left_x, lower_left_y,
               image_width, image_height);
    lower_left_x = gl_state.screen_width - image_width - 250;
    image_t sph_state;
    init_image(&sph_state, 
               &gl_state, 
               "TinyLauncher/sph.png", 
               "TinyLauncher/sph-selected.png",
               lower_left_x, lower_left_y,
               image_width, image_height);
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
        // Update center of cursor
        set_cursor_position(&cursor_state, state.cursor_x, state.cursor_y);

        // Check if anything selected
        check_cursor_in_image(&cursor_state, &mandelbrot_state);
        check_cursor_in_image(&cursor_state, &sph_state);

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

// Check if cursor is inside of image, if so set selected attribute
void check_cursor_in_image(cursor_t *cursor_state, image_t *image_state)
{
    int x, y;
    int half_width = cursor_state->cursor_width/2;
    int half_height = cursor_state->cursor_height/2;
    // Upper left corner
    x = cursor_state->center_x - half_width;
    y = cursor_state->center_y + half_height;
    if( inside_image(image_state, x, y) ){
        image_state->selected = true;
        return;  
    }
    // Lower left corner
    x = cursor_state->center_x - half_width;
    y = cursor_state->center_y - half_height;
    if( inside_image(image_state, x, y) ){
        image_state->selected = true;
        return;
    }  
    // Upper right corner
    x = cursor_state->center_x + half_width;
    y = cursor_state->center_y + half_height;
    if( inside_image(image_state, x, y) ){
        image_state->selected = true;
        return;
    }  
    // Lower right corner
    x = cursor_state->center_x + half_width;
    y = cursor_state->center_y - half_height;
    if( inside_image(image_state, x, y) ){
        image_state->selected = true;
        return;
    }

    image_state->selected = false;
}

void update_cursor(launcher_t *state, int x, int y)
{
    // Update launcher state
    state->cursor_x = x;
    state->cursor_y = y;
}

