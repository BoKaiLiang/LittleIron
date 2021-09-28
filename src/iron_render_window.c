#include "iron_render_window.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"

#include "iron_util.h"
#include "iron_asset.h"
#include "iron_math.h"

// default vertex sahder

// default fragment shader
static const char* DEFAULT_2D_VERTEX_SHADER_CODE = "#version 330 core\n"
"in vec2 _Pos;\n"
"in vec2 _Texcoords;\n"
"out vec2 Texcoords;\n"
"uniform mat4 _MVP;\n"
"void main() {\n"
"   gl_Position = _MVP * vec4(_Pos, 0.0, 1.0);\n"
"   Texcoords = _Texcoords;\n"
"}\n\0";

static const char* DEFAULT_2D_FRAGMENT_SHADER_CODE = "#version 330 core\n"
"in vec2 Texcoords;\n"
"uniform vec4 _Color;\n"
"uniform sampler2D _Texture2D;"
"out vec4 _FragColor;\n"
"void main() {\n"
"   _FragColor = _Color * texture(_Texture2D, Texcoords);\n"
"}\n\0";


// tempoary...
//-------------vertices data------------- 
static const float VERTICES[] = {
	0.5f,  0.5f, 1.0f, 1.0f,	// top right
 	0.5f, -0.5f, 1.0f, 0.0f, 	// bottom right
	-0.5f, -0.5f, 0.0f, 0.0f, 	// bottom left
	-0.5f, 0.5f, 0.0f, 1.0f	// top left 
};

static const unsigned int INDICES[] = {
	0, 1, 3,   
	1, 2, 3
};

// all window and renderer state data
static struct {
    struct {
        int w, h;
        GLFWwindow* handle;
    } Window;

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
        float frame_time;
        float update_time;
        float render_time;
    } Time;

    struct {
        unsigned vbo, vao, ibo;

        Shader default_shader;
        Texture default_texture;

        Mat4 projection;
    } RenderState;

} CONTEXT;

// ---------------------------------- //
//        Functions Declaration       //
// ---------------------------------- //

// offical doc: https://www.glfw.org/docs/3.3/input_guide.html#input_key

// [iron_render_window] callback function for glfw window to handle our keyboard input
static void WindowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
// [iron_render_window] callback function for glfw window to handle mouse button
static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
// [iron_render_window] callback function for glfw window to handle mouse cursor position
static void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
// [iron_render_window] callback function for glfw window to handle mouse scroll yoffset
static void MouseScrollCallback(GLFWwindow* window, double xpox, double ypos);
// [iron_render_widnow] callcak function when resize window size
static void FramebufferSizeCallback(GLFWwindow* window, int w, int h);

// ---------------------------------- //
//          Window Management         //
// ---------------------------------- //

// [iron_render_window] create window handle and 2d renderer
ResT CreateRenderWindow(int w, int h, const char* title, int fps) {
    // glfw window
    if (!glfwInit()) {
        LogError("Failed to init glfw.")
        return RES_ERROR_CREATE_WINDOW;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    CONTEXT.Window.w = w;
    CONTEXT.Window.h = h;
    CONTEXT.Window.handle = glfwCreateWindow(w, h, title, NULL, NULL);
    if (CONTEXT.Window.handle == NULL) {
        LogError("Failed to create glfw window.")
        return RES_ERROR_CREATE_WINDOW;
    }
    glfwMakeContextCurrent(CONTEXT.Window.handle);

    glfwSetKeyCallback(CONTEXT.Window.handle, WindowKeyCallback);
    glfwSetMouseButtonCallback(CONTEXT.Window.handle, MouseButtonCallback);
    glfwSetCursorPosCallback(CONTEXT.Window.handle, CursorPositionCallback);
    glfwSetScrollCallback(CONTEXT.Window.handle, MouseScrollCallback);
    glfwSetFramebufferSizeCallback(CONTEXT.Window.handle, FramebufferSizeCallback);

    // input
    for (int i = 0; i < MAX_KEY_CODE; i++) {
        CONTEXT.Keyboard.key_current_state[i] = 0;
        CONTEXT.Keyboard.key_previous_state[i] = 0;
    }

    for (int i = 0; i < MAX_MOUSE_BUTTON; i++) {
        CONTEXT.Mouse.mouse_btn_current_state[i] = 0;
        CONTEXT.Mouse.mouse_btn_previous_state[i] = 0;
    }
    CONTEXT.Mouse.cursor_pos = (V2f){ 0.0f, 0.0f };
    CONTEXT.Mouse.scroll_offset = 0.0f;

    // time
    CONTEXT.Time.previous_time = (float)glfwGetTime();
    CONTEXT.Time.current_time = (float)glfwGetTime();
    CONTEXT.Time.target_time = (float)(1.0f / fps);

    // 2D renderer
    // init glad
    if (!gladLoadGL(glfwGetProcAddress)) {
		LogError("Failed to load glad.");
        return RES_ERROR_LOAD_GLAD;
    }

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // load default shader
    ResT load_shader_res = LoadShaderCode(&CONTEXT.RenderState.default_shader,
        DEFAULT_2D_VERTEX_SHADER_CODE, 
        DEFAULT_2D_FRAGMENT_SHADER_CODE);

    if (load_shader_res != RES_SUCCESS) {
        LogError("Failed to load default shader");
        return load_shader_res;
    }

    // init vertex buffer object, vertex array object and index buffer object
	glGenVertexArrays(1, &CONTEXT.RenderState.vao);
	glBindVertexArray(CONTEXT.RenderState.vao);

	glGenBuffers(1, &CONTEXT.RenderState.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, CONTEXT.RenderState.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

	glGenBuffers(1, &CONTEXT.RenderState.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CONTEXT.RenderState.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INDICES), INDICES, GL_STATIC_DRAW);

	// set position attribute
	glVertexAttribPointer(CONTEXT.RenderState.default_shader.attribs_locations[SHADER_ATTRIB_VEC2_POS], 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(CONTEXT.RenderState.default_shader.attribs_locations[SHADER_ATTRIB_VEC2_POS]);

	// set texture coordinate attribute
	glVertexAttribPointer(CONTEXT.RenderState.default_shader.attribs_locations[SHADER_ATTRIB_VEC2_TEXCOORD], 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(CONTEXT.RenderState.default_shader.attribs_locations[SHADER_ATTRIB_VEC2_TEXCOORD]);

	glBindVertexArray(0);

	// create default texture, a pixel 1x1 white squad
	unsigned char white_pixel[4] = { 255, 255, 255, 255 };
	CONTEXT.RenderState.default_texture.w = 1;
	CONTEXT.RenderState.default_texture.h = 1;
	CONTEXT.RenderState.default_texture.gl_fmt = TEX_FMT_RGBA;

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &CONTEXT.RenderState.default_texture.id);
	glBindTexture(GL_TEXTURE_2D, CONTEXT.RenderState.default_texture.id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white_pixel);

	glBindTexture(GL_TEXTURE_2D, 0);

    // math... random
    srand((time_t)NULL);

    return RES_SUCCESS;
}

// [iron_render_window] release window handle and 2d renderer
void ReleaseRenderWindow(void) {
    glDeleteBuffers(1, &CONTEXT.RenderState.vbo);
    glDeleteBuffers(1, &CONTEXT.RenderState.ibo);
    glDeleteVertexArrays(1, &CONTEXT.RenderState.vao);
    glDeleteTextures(1, &CONTEXT.RenderState.default_texture.id);
    glDeleteProgram(CONTEXT.RenderState.default_shader.id);

    if (CONTEXT.Window.handle != NULL) {
        glfwDestroyWindow(CONTEXT.Window.handle);
    }
    glfwTerminate();
}

// [iron_render_window] check is widnow running
bool IsWindowRunning(void) {
    return !glfwWindowShouldClose(CONTEXT.Window.handle);
}

// [iron_render_window] release window handle and 2d renderer
void CloseRenderWindow(void) {
    glfwSetWindowShouldClose(CONTEXT.Window.handle, 1);
}

// [iron_render_window] state of state rendering and store logic update time step
void BeginRednering(Color clear_color) {
    // update time
    CONTEXT.Time.current_time = (float)glfwGetTime();
    CONTEXT.Time.update_time = CONTEXT.Time.current_time - CONTEXT.Time.previous_time;
    CONTEXT.Time.previous_time = CONTEXT.Time.current_time;

    // clear frame and draw background
    glClear(GL_COLOR_BUFFER_BIT);
    V4f color = ColorToVec4f(clear_color);
    glClearColor(color.r, color.g, color.b, color.a);

    // update projection matrix
	CONTEXT.RenderState.projection = Mat4Ortho(0.0f, (float)CONTEXT.Window.w, (float)CONTEXT.Window.h, 0.0f, -1.0f, 1.0f);
}

// [iron_render_window] state of end rendering and store drawing time step, frame time step
void EndRendering() {
    glfwSwapBuffers(CONTEXT.Window.handle);

    // reset input state
    // must put before call `glfwPollEvents`

    // keyboard
    for (int i = 0; i < MAX_KEY_CODE; i++) {
        CONTEXT.Keyboard.key_previous_state[i] = CONTEXT.Keyboard.key_current_state[i];
    }

    // mouse
    for (int i = 0; i < MAX_MOUSE_BUTTON; i++) {
        CONTEXT.Mouse.mouse_btn_previous_state[i] = CONTEXT.Mouse.mouse_btn_current_state[i];
    }
    CONTEXT.Mouse.scroll_offset = 0.0f;

    glfwPollEvents();

    // render time
    CONTEXT.Time.current_time = (float)glfwGetTime();
    CONTEXT.Time.render_time = CONTEXT.Time.current_time - CONTEXT.Time.previous_time;
    CONTEXT.Time.previous_time = CONTEXT.Time.current_time;

    CONTEXT.Time.frame_time = CONTEXT.Time.render_time + CONTEXT.Time.update_time;

    if (CONTEXT.Time.frame_time < CONTEXT.Time.target_time) {
        float wait_time = CONTEXT.Time.target_time - CONTEXT.Time.frame_time;

        // wait time
        float start_wait_time = glfwGetTime();
        float end_wait_time = 0.0f;
        while ((end_wait_time - start_wait_time) < wait_time) {
            end_wait_time = glfwGetTime();
        }

        CONTEXT.Time.current_time = (float)glfwGetTime();
        float extra_time = CONTEXT.Time.current_time - CONTEXT.Time.previous_time;
        CONTEXT.Time.previous_time = CONTEXT.Time.current_time;

        CONTEXT.Time.frame_time += extra_time;
    }
}

// ---------------------------------- //
//              Input                 //
// ---------------------------------- //

// [iron_render_window] check is key pressed
bool IsKeyPressed(KeyCode keycode) {
    if (CONTEXT.Keyboard.key_previous_state[keycode] == 0 && CONTEXT.Keyboard.key_current_state[keycode] == 1)
        return true;
    return false;
}

// [iron_render_window] check is key released after pressed
bool IsKeyReleased(KeyCode keycode) {
    if (CONTEXT.Keyboard.key_previous_state[keycode] == 1 && CONTEXT.Keyboard.key_current_state[keycode] == 0)
        return true;
    return false;
}

// [iron_render_window] check is key down
bool IsKeyDown(KeyCode keycode) {
    if (CONTEXT.Keyboard.key_current_state[keycode] == 1)
        return true;
    return false;
}

// [iron_render_window] check is key up
bool IsKeyUp(KeyCode keycode) {
    if (CONTEXT.Keyboard.key_current_state[keycode] == 0)
        return true;
    return false;
}

// [iron_render_window] check is mouse button pressed
bool IsMouseButtonPressed(MouseButton mouse_button) {
    if (CONTEXT.Mouse.mouse_btn_previous_state[mouse_button] == 0 && CONTEXT.Mouse.mouse_btn_current_state[mouse_button] == 1)
        return true;
    return false;
}

// [iron_render_window] check is mouse button released after pressed
bool IsMouseButtonReleased(MouseButton mouse_button) {
    if (CONTEXT.Mouse.mouse_btn_previous_state[mouse_button] == 1 && CONTEXT.Mouse.mouse_btn_current_state[mouse_button] == 0)
        return true;
    return false;
}

// [iron_render_window] get cursor position
V2f GetMousePosition() {
    return CONTEXT.Mouse.cursor_pos;
}

// [iron_render_window] get mouse scroll move
float GetScrollYOffset() {
    return CONTEXT.Mouse.scroll_offset;
}

// ---------------------------------- //
//                Time                //
// ---------------------------------- //

// [iron_render_window] get game loop time
float GetTime() {
    return (float)glfwGetTime();
}

// [iron_render_window] get delta time(frame time)
float GetDeltaTime() {
    return CONTEXT.Time.frame_time;
}

// ---------------------------------- //
//               Render               //
// ---------------------------------- //

// [iron_render_window] draw rectangle
void DrawRectangle(V2f pos, V2f sz, float angle, Color c) {
    glBindVertexArray(CONTEXT.RenderState.vao);

	glUseProgram(CONTEXT.RenderState.default_shader.id);

	// set mvp matrix
	Mat4 mvp_matrix = MAT4_IDENTITY;

	// translate > rotate > scale
	mvp_matrix = Mat4Translate(mvp_matrix, pos.x, pos.y);
	mvp_matrix = Mat4EulerRotate(mvp_matrix, angle);
	mvp_matrix = Mat4Scale(mvp_matrix, sz.x, sz.y);

	// model * projection
 	mvp_matrix = Mat4MulMat4(CONTEXT.RenderState.projection, mvp_matrix);

	glUniformMatrix4fv(CONTEXT.RenderState.default_shader.attribs_locations[SHADER_ATTRIB_MAT4_MVP], 1, GL_FALSE, mvp_matrix.unit);

	V4f v = ColorToVec4f(c);
	glUniform4f(CONTEXT.RenderState.default_shader.attribs_locations[SHADER_ATTRIB_VEC4_COLOR], v.r, v.g, v.b, v.a);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, CONTEXT.RenderState.default_texture.id);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

// [iron_render_window] draw line
void DrawLine(V2f p0, V2f p1, int thinkness, Color c) {
    // TODO:
}

// ---------------------------------- //
//          Static Functions          //
// ---------------------------------- //

// [iron_render_window] callback function for glfw window to handle our keuboard input
static void WindowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_RELEASE) {
        CONTEXT.Keyboard.key_current_state[key] = 0;
    } else if (action == GLFW_PRESS) {
        CONTEXT.Keyboard.key_current_state[key] = 1;
    }
}

// [iron_render_window] callback function for glfw window to handle mouse button
static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_RELEASE) {
        CONTEXT.Mouse.mouse_btn_current_state[button] = 0;
    } else if (action == GLFW_PRESS) {
        CONTEXT.Mouse.mouse_btn_current_state[button] = 1;
    }
}

// [iron_render_window] callback function for glfw window to handle mouse cursor position
static void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    CONTEXT.Mouse.cursor_pos.x = (float)xpos;
    CONTEXT.Mouse.cursor_pos.y = (float)ypos;
}

// [iron_render_window] callback function for glfw window to handle mouse scroll yoffset
static void MouseScrollCallback(GLFWwindow* window, double xpox, double ypos) {
    CONTEXT.Mouse.scroll_offset = (float)ypos;
}

// [iron_widnow] callcak function when resize window size
static void FramebufferSizeCallback(GLFWwindow* window, int w, int h) {
    CONTEXT.Window.w = w;
    CONTEXT.Window.h = h;
    glViewport(0, 0, w, h);
}