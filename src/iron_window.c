#include "iron_window.h"

#include <stdio.h>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

// #define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"

#include "iron_util.h"

// static struct: 
// -- only show in this module, can be seen as 'private' variable.
static struct {
    GLFWwindow* wnd;
} WINDOW;

// [iron_window] callback function for glfw window to handle our keuboard input
// -- offical doc: https://www.glfw.org/docs/3.3/input_guide.html#input_key
static void WindowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

// ---------------------------------- //
//      Functions implementaion       //
// ---------------------------------- //

// [iron_window] create window
ResT CreateWindow(int w, int h, const char* title) {
    if (!glfwInit()) {
        printf("Failed to init glfw.");
        return RES_ERROR_CREATE_WINDOW;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    WINDOW.wnd = glfwCreateWindow(w, h, title, NULL, NULL);
    if (WINDOW.wnd == NULL) {
        printf("Failed to create glfw window.");
        return RES_ERROR_CREATE_WINDOW;
    }
    glfwMakeContextCurrent(WINDOW.wnd);

    glfwSetKeyCallback(WINDOW.wnd, WindowKeyCallback);      // handle key input

    return RES_SUCCESS;
}

// [iron_window] release window
void ReleaseWindow() {
    if (WINDOW.wnd != NULL) {
        glfwDestroyWindow(WINDOW.wnd);
    }
    glfwTerminate();
}

// [iron_window] check is window should be close or not
int IsWindowRunning(void) {
    return !glfwWindowShouldClose(WINDOW.wnd);
}

// [iron_window] close window
void CloseWindow(void) {
    glfwSetWindowShouldClose(WINDOW.wnd, 1);
}

// [iron_window] first state in game loop, refresh the scene
void StartScene(Color clear_color) {
    glClear(GL_COLOR_BUFFER_BIT);
    V4f colorv = ColorToVec4f(clear_color);
    glClearColor(colorv.r, colorv.g, colorv.b, colorv.a);
}

// [iron_window] last state in game loop, store scene data in this frame
void EndScene() {
    glfwSwapBuffers(WINDOW.wnd);
    glfwPollEvents();
}

// [iron_window] callback function for glfw window to handle our keuboard input
static void WindowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    // NOTE: close window by esacpe key
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE) {
            CloseWindow();
        }
    }
}