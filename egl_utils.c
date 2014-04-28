/*
The MIT License (MIT)

Copyright (c) 2014 Adam Simpson

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

/*
Copyright (c) 2012, Broadcom Europe Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "egl_utils.h"
#include "linux/input.h"

#include "launcher.h"

bool window_should_close(gl_t *state)
{
    if(state->window_should_close)
        return true;
    else
        return false;
}

// Description: Sets the display, OpenGL|ES context and screen stuff
void init_ogl(gl_t *state, void *launcher_state)
{
    bcm_host_init();

    // Initialize struct
    memset(state, 0, sizeof(gl_t));
    state->controller_1_fd = -1;
    state->controller_2_fd = -1;
    state->window_should_close = false;

    // Set a user pointer up
    state->user_pointer = launcher_state;

    int32_t success = 0;
    EGLBoolean result;
    EGLint num_config;

    static EGL_DISPMANX_WINDOW_T nativewindow;

    DISPMANX_ELEMENT_HANDLE_T dispman_element;
    DISPMANX_DISPLAY_HANDLE_T dispman_display;
    DISPMANX_UPDATE_HANDLE_T dispman_update;
    VC_RECT_T dst_rect;
    VC_RECT_T src_rect;

    static const EGLint attribute_list[] =
    {
       EGL_RED_SIZE, 8,
       EGL_GREEN_SIZE, 8,
       EGL_BLUE_SIZE, 8,
       EGL_ALPHA_SIZE, 8,
       EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
       EGL_NONE
    };
   
    static const EGLint context_attributes[] =
    {
       EGL_CONTEXT_CLIENT_VERSION, 2,
       EGL_NONE
    }; 

    EGLConfig config;

    // get an EGL display connection
    state->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    assert(state->display!=EGL_NO_DISPLAY);
 
    // initialize the EGL display connection
    result = eglInitialize(state->display, NULL, NULL);
    assert(EGL_FALSE != result);

    // get an appropriate EGL frame buffer configuration
    result = eglChooseConfig(state->display, attribute_list, &config, 1, &num_config);
    assert(EGL_FALSE != result);

    // get an appropriate EGL frame buffer configuration
    result = eglBindAPI(EGL_OPENGL_ES_API);
    assert(EGL_FALSE != result);

    // create an EGL rendering context
    state->context = eglCreateContext(state->display, config, EGL_NO_CONTEXT, context_attributes);
    assert(state->context!=EGL_NO_CONTEXT);

    // create an EGL window surface
    success = graphics_get_display_size(0 /* LCD */, &state->screen_width, &state->screen_height);
    assert( success >= 0 );

    dst_rect.x = 0;
    dst_rect.y = 0;
    dst_rect.width = state->screen_width;
    dst_rect.height = state->screen_height;
      
    src_rect.x = 0;
    src_rect.y = 0;
    src_rect.width = state->screen_width << 16;
    src_rect.height = state->screen_height << 16;

    dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
    dispman_update = vc_dispmanx_update_start( 0 );

    VC_DISPMANX_ALPHA_T alpha;
    alpha.flags = DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS;
    alpha.opacity = 255;
    alpha.mask = 0;         

    dispman_element = vc_dispmanx_element_add ( dispman_update, dispman_display,
      0/*layer*/, &dst_rect, 0/*src*/,
      &src_rect, DISPMANX_PROTECTION_NONE, &alpha, 0/*clamp*/, 0/*transform*/);
      
    nativewindow.element = dispman_element;
    nativewindow.width = state->screen_width;
    nativewindow.height = state->screen_height;
    vc_dispmanx_update_submit_sync( dispman_update );
      
    state->surface = eglCreateWindowSurface( state->display, config, &nativewindow, NULL );
    assert(state->surface != EGL_NO_SURFACE);

    // connect the context to the surface
    result = eglMakeCurrent(state->display, state->surface, state->surface, state->context);
    assert(EGL_FALSE != result);

    // Open input event
    state->controller_1_fd = open("/dev/input/event1",O_RDONLY|O_NONBLOCK);
    state->controller_2_fd = open("/dev/input/event2",O_RDONLY|O_NONBLOCK);
}

void swap_ogl(gl_t *state)
{
    eglSwapBuffers(state->display, state->surface);
}

void exit_ogl(gl_t *state)
{
   // clear screen
   glClear( GL_COLOR_BUFFER_BIT );
   eglSwapBuffers(state->display, state->surface);

   // Release OpenGL resources
   eglMakeCurrent( state->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
   eglDestroySurface( state->display, state->surface );
   eglDestroyContext( state->display, state->context );
   eglTerminate( state->display );

   close(state->controller_2_fd);
   close(state->controller_1_fd);

   printf("close\n");
}

// Handle key press
void handle_key(gl_t *state, struct input_event *event)
{
    // Get launcher_state from gl_state
    launcher_t *launcher_state = (launcher_t*)state->user_pointer;

    // Recognize single key press events
    if(event->value == 1 && event->code > 0)
    {
        switch(event->code)
        {
            case KEY_A:
                launch_selected_program(launcher_state);
                break;
            default:
                break;
        }
    }
}

// Handle mouse movement
void handle_mouse(gl_t *state, struct input_event *event)
{
    // Get launcher_state from gl_state
    launcher_t *launcher_state = (launcher_t*)state->user_pointer;

    // Initialize mouse position
    static int x = 1920/2;
    static int y = 1080/2;

    // Handle mouse movement
    switch(event->code)
    {
        case REL_X:
            x += 2*event->value;
            // Make sure not to go out of bounds
            if(x < 0.0f)
                x = 0.0f;
            else if(x > state->screen_width)
                x = state->screen_width;
            update_cursor(launcher_state, x, y);
            break;
        case REL_Y:
            y += 2*event->value;
            if(y < 0.0f)
                y = 0.0f;
            else if(y > state->screen_height)
                y = state->screen_height;
            update_cursor(launcher_state, x, y);
            break;
        case REL_WHEEL:
            if(event->value > 0.0f)
                break;
            else if(event->value < 0.0f)
                break;
            break;
        case REL_HWHEEL:
            if(event->value > 0.0f)
                break;
            else if(event->value < 0.0f)
                break;
            break;
    }
}

void handle_joystick(gl_t *state, struct input_event *event)
{
    printf("No joystick handling\n");    
}

void process_controller_events(gl_t *state, int controller_fd)
{
    struct input_event events[10];
    int bytes, i, length;

    // Read in events
    bytes = read(controller_fd, events, sizeof(events));
    if(bytes > 0)
    {
        length =  bytes/sizeof(struct input_event);

        // Process events based on type
        for(i=0; i<length; i++)
        {
            switch(events[i].type)
            {
                case EV_KEY:
                    handle_key(state, &events[i]);
                    break;
                case EV_ABS:
                    handle_joystick(state, &events[i]);
                    break;
                case EV_REL:
                    handle_mouse(state, &events[i]);
                    break;
            }
        }
    }
}

// Convert pixel coordinates, lower left origin, to gl coordinates, center origin
void pixel_to_gl(gl_t *state, int pixel_x, int pixel_y, float *gl_x, float *gl_y)
{
    float half_x = state->screen_width/2.0;
    float half_y = state->screen_height/2.0;
    *gl_x = pixel_x/half_x - 1.0;
    *gl_y = pixel_y/half_y - 1.0;

}

// Poll /dev/input for any input event
// https://www.kernel.org/doc/Documentation/input/input.txt
void check_user_input(gl_t *state)
{
    // If controllers are open process their events
    if(state->controller_1_fd > 0)
        process_controller_events(state, state->controller_1_fd);
    if(state->controller_2_fd > 0)
        process_controller_events(state, state->controller_2_fd);
}
