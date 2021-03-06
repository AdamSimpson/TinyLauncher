#include <stdio.h>
#include <unistd.h>
#include "launcher.h"
#include "egl_utils.h"
#include "image_gl.h"
#include "cursor_gl.h"

int main(int argc, char *argv[])
{
    // Create state for launcher program
    launcher_t state;

    // Setup initial OpenGL state
    gl_t gl_state;
    memset(&gl_state, 0, sizeof(gl_t));
    init_ogl(&gl_state, &state);

    // Initialize program launcher images OpenGL state
    int image_width = 600;
    int image_height = 375;
    int lower_left_y = gl_state.screen_height/2 - image_height/2;
    int lower_left_x = 250;
    state.mandelbrot_state = malloc(sizeof(image_t));
    init_image(state.mandelbrot_state, 
               &gl_state, 
               "TinyLauncher/mandelbrot.png",
               "TinyLauncher/mandelbrot-selected.png",
               lower_left_x, lower_left_y,
               image_width, image_height);
    lower_left_x = gl_state.screen_width - image_width - 250;
    state.sph_state = malloc(sizeof(image_t));
    init_image(state.sph_state, 
               &gl_state, 
               "TinyLauncher/sph.png", 
               "TinyLauncher/sph-selected.png",
               lower_left_x, lower_left_y,
               image_width, image_height);
    // Initialize cursor
    state.cursor_state = malloc(sizeof(cursor_t));
    int cursor_width = 30;
    int cursor_height = 40;
    init_cursor(state.cursor_state, &gl_state, "TinyLauncher/cursor.png", cursor_width, cursor_height);

    // Set background color
    glClearColor(1.0, 1.0, 1.0, 1.0);

    while(1)
    {
        // Update user input
        check_user_input(&gl_state);
        // Update center of cursor
        set_cursor_position(state.cursor_state, state.cursor_state->center_x, state.cursor_state->center_y);

        // Check if anything selected
        check_cursor_in_image(state.cursor_state, state.mandelbrot_state);
        check_cursor_in_image(state.cursor_state, state.sph_state);

        // Clear background
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw mandelbrot image
        draw_image(state.mandelbrot_state);

        // Draw SPH image
        draw_image(state.sph_state);

        // Draw cursor
        draw_cursor(state.cursor_state);

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
    state->cursor_state->center_x = x;
    state->cursor_state->center_y = y;
}

// Launch a program if one selected
void launch_selected_program(launcher_t *launcher_state)
{
    if(launcher_state->mandelbrot_state->selected)
        launch_mandelbrot(launcher_state);  
    else if (launcher_state->sph_state->selected)
        launch_sph(launcher_state);
}

void launch_sph(launcher_t *state)
{
    system("mpirun -f /home/pi/pi_mpihostsfile -n 9 /home/pi/sph.out");
}

void launch_mandelbrot(launcher_t *state)
{
    system("mpirun -f /home/pi/pi_mpihostsfile -n 9 /home/pi/pibrot");
}
