#include "iron_window.h"

#include <stdio.h>
#include <stdlib.h>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

// #define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"

#include "iron_util.h"

// static struct: 
// -- only show in this module, can be seen as 'private' variable.
static struct {
    GLFWwindow* wnd;
    struct {
        int key_current_state[MAX_KEY_CODE];
        int key_previous_state[MAX_KEY_CODE];
    } Keyboard;

    struct {
        int mouse_btn_current_state[MAX_MOUSE_BUTTON];
        int mouse_btn_previous_state[MAX_MOUSE_BUTTON];
        V2f cursor_pos;
        float scroll_offset;
    } Mouse;

    struct {
        float target_time;
        float current_time;
        float previous_time;
        float delta_time;
    } Time;
} WINDOW;

// offical doc: https://www.glfw.org/docs/3.3/input_guide.html#input_key

// [iron_window] callback function for glfw window to handle our keyboard input
static void WindowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
// [iron_window] callback function for glfw window to handle mouse button
static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
// [iron_window] callback function for glfw window to handle mouse cursor position
static void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
// [iron_window] callback function for glfw window to handle mouse scroll yoffset
static void MouseScrollCallback(GLFWwindow* window, double xpox, double ypos);
// [iron_widnow] callcak function when resize window size
static void FramebufferSizeCallback(GLFWwindow* window, int w, int h);

// ---------------------------------- //
//      Functions implementaion       //
// ---------------------------------- //

// [iron_window] create window
ResT CreateWindow(int w, int h, const char* title) {
    if (!glfwInit()) {
        LogError("Failed to init glfw.")
        return RES_ERROR_CREATE_WINDOW;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    WINDOW.wnd = glfwCreateWindow(w, h, title, NULL, NULL);
    if (WINDOW.wnd == NULL) {
        LogError("Failed to create glfw window.")
        return RES_ERROR_CREATE_WINDOW;
    }
    glfwMakeContextCurrent(WINDOW.wnd);

    glfwSetKeyCallback(WINDOW.wnd, WindowKeyCallback);
    glfwSetMouseButtonCallback(WINDOW.wnd, MouseButtonCallback);
    glfwSetCursorPosCallback(WINDOW.wnd, CursorPositionCallback);
    glfwSetScrollCallback(WINDOW.wnd, MouseScrollCallback);
    glfwSetFramebufferSizeCallback(WINDOW.wnd, FramebufferSizeCallback);

    for (int i = 0; i < MAX_KEY_CODE; i++) {
        WINDOW.Keyboard.key_current_state[i] = 0;
        WINDOW.Keyboard.key_previous_state[i] = 0;
    }

    for (int i = 0; i < MAX_MOUSE_BUTTON; i++) {
        WINDOW.Mouse.mouse_btn_current_state[i] = 0;
        WINDOW.Mouse.mouse_btn_previous_state[i] = 0;
    }
    WINDOW.Mouse.cursor_pos = (V2f){ 0.0f, 0.0f };
    WINDOW.Mouse.scroll_offset = 0.0f;

    // time
    WINDOW.Time.previous_time = 0.0f;
    WINDOW.Time.current_time = 0.0f;
    WINDOW.Time.target_time = 0.0f;

    // math... random
    srand((time_t)NULL);

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

// [iron_window] get window height and weight
V2f GetWindowWH() {
    int w, h;
    glfwGetWindowSize(WINDOW.wnd, &w, &h);
    return (V2f){ (float)w, (float)h };
}

// [iron_window] first state in game loop, refresh the scene
void StartScene(Color clear_color) {
    glClear(GL_COLOR_BUFFER_BIT);
    V4f colorv = ColorToVec4f(clear_color);
    glClearColor(colorv.r, colorv.g, colorv.b, colorv.a);

    // time
    WINDOW.Time.previous_time = WINDOW.Time.current_time;
    WINDOW.Time.current_time = (float)glfwGetTime();
    WINDOW.Time.delta_time = WINDOW.Time.current_time - WINDOW.Time.previous_time;
}

// [iron_window] last state in game loop, store scene data in this frame
void EndScene() {
    glfwSwapBuffers(WINDOW.wnd);

    // reset input state
    // must put before call `glfwPollEvents`

    // keyboard
    for (int i = 0; i < MAX_KEY_CODE; i++) {
        WINDOW.Keyboard.key_previous_state[i] = WINDOW.Keyboard.key_current_state[i];
    }

    // mouse
    for (int i = 0; i < MAX_MOUSE_BUTTON; i++) {
        WINDOW.Mouse.mouse_btn_previous_state[i] = WINDOW.Mouse.mouse_btn_current_state[i];
    }
    WINDOW.Mouse.scroll_offset = 0.0f;

    glfwPollEvents();
}

// [iron_window] check is key pressed
int IsKeyPressed(KeyCode keycode) {
    if (WINDOW.Keyboard.key_previous_state[keycode] == 0 && WINDOW.Keyboard.key_current_state[keycode] == 1)
        return 1;
    return 0;
}

// [iron_window] check is key released after pressed
int IsKeyReleased(KeyCode keycode) {
    if (WINDOW.Keyboard.key_previous_state[keycode] == 1 && WINDOW.Keyboard.key_current_state[keycode] == 0)
        return 1;
    return 0;
}

// [iron_window] check is key up
int IsKeyDown(KeyCode keycode) {
    if (WINDOW.Keyboard.key_current_state[keycode] == 1)
        return 1;
    return 0;
}

// [iron_window] check is key down
int IsKeyUp(KeyCode keycode) {
    if (WINDOW.Keyboard.key_current_state[keycode] == 0)
        return 1;
    return 0;
}

// [iron_window] check is mouse button pressed
int IsMouseButtonPressed(MouseButton mouse_button) {
    if (WINDOW.Mouse.mouse_btn_previous_state[mouse_button] == 0 && WINDOW.Mouse.mouse_btn_current_state[mouse_button] == 1)
        return 1;
    return 0;
}

// [iron_window] check is mouse button released
int IsMouseButtonReleased(MouseButton mouse_button) {
    if (WINDOW.Mouse.mouse_btn_previous_state[mouse_button] == 1 && WINDOW.Mouse.mouse_btn_current_state[mouse_button] == 0)
        return 1;
    return 0;
}

// [iron_window] get mouse cursor position
V2f GetMousePosition() {
    return WINDOW.Mouse.cursor_pos;
}

// [iron_window] get mouse scroll roll
float GetScrollYOffset() {
    return WINDOW.Mouse.scroll_offset;
}

void SetTargetFPS(int fps) {
    WINDOW.Time.target_time = 1.0f / (float)fps;
}

// [iron_window] get time
float GetTime() {
    return (float)glfwGetTime();
}

// [iron_window] get delta time
float GetDeltaTime() {
    return WINDOW.Time.delta_time;
}

// ---------------------------------- //
//          Static Functions          //
// ---------------------------------- //

// [iron_window] callback function for glfw window to handle our keuboard input
static void WindowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    // NOTE: close window by esacpe key
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE) {
            CloseWindow();
        }
    }

    if (action == GLFW_RELEASE) {
        WINDOW.Keyboard.key_current_state[key] = 0;
    } else if (action == GLFW_PRESS) {
        WINDOW.Keyboard.key_current_state[key] = 1;
    }
}

// [iron_window] callback function for glfw window to handle mouse button
static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_RELEASE) {
        WINDOW.Mouse.mouse_btn_current_state[button] = 0;
    } else if (action == GLFW_PRESS) {
        WINDOW.Mouse.mouse_btn_current_state[button] = 1;
    }
}

// [iron_window] callback function for glfw window to handle mouse cursor position
static void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    WINDOW.Mouse.cursor_pos.x = (float)xpos;
    WINDOW.Mouse.cursor_pos.y = (float)ypos;
}

// [iron_window] callback function for glfw window to handle mouse scroll yoffset
static void MouseScrollCallback(GLFWwindow* window, double xpox, double ypos) {
    WINDOW.Mouse.scroll_offset = (float)ypos;
}

// [iron_widnow] callcak function when resize window size
static void FramebufferSizeCallback(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, w, h);
}