#include "iron_render_window.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"

#include "iron_util.h"
#include "iron_asset.h"
#include "iron_math.h"

#define VERICES_PER_RECT 4
#define TEXCOORDS_PER_RECT 4
#define COLORS_PER_RECT 4
#define INDICES_PER_RECT 6
#define DEFAULT_MAX_RECT 4096
#define MAX_DRAW_CALL_PER_FRAME 256

// default vertex sahder
static const char* DEFAULT_2D_VERTEX_SHADER_CODE = "#version 330 core\n"
"in vec2 _Pos;\n"
"in vec2 _Texcoords;\n"
"in vec4 _Color;\n"
"out vec2 Texcoords;\n"
"out vec4 Color;\n"
"uniform mat4 _MVP;\n"
"void main() {\n"
"   gl_Position = _MVP * vec4(_Pos, 0.0, 1.0);\n"
"   Texcoords = _Texcoords;\n"
"   Color = _Color;\n"
"}\n\0";

// * texture(_Texture2D, Texcoords)

// default fragment shader
static const char* DEFAULT_2D_FRAGMENT_SHADER_CODE = "#version 330 core\n"
"in vec2 Texcoords;\n"
"in vec4 Color;\n"
"uniform sampler2D _Texture2D;"
"out vec4 _FragColor;\n"
"void main() {\n"
"   _FragColor = Color * texture(_Texture2D, Texcoords);\n" // TODO: can't draw the default texture over here
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

typedef struct VertexBuffer {
    V2f* vertices;
    size_t vertices_count;
    size_t vertices_max;

    V2f* texcoords;
    size_t texcoords_count;
    size_t texcoords_max;

    V4f* colors;
    size_t colors_count;
    size_t colors_max;

    unsigned int* indices;
    size_t indices_count;
    size_t indices_max;

    unsigned int vbo[4]; // positions, texture coordinates, colors, indicess
    unsigned int vao;
} VertexBuffer;

typedef struct DrawCall {
    size_t vertices_count;
    unsigned int texture_id;
} DrawCall;

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

        int fps;
        int fps_count;
        float fps_time_counter;
    } Time;

    struct {
        VertexBuffer vertex_buffer;

        DrawCall draw_calls[MAX_DRAW_CALL_PER_FRAME];
        unsigned int draw_calls_count;

        Shader default_shader;
        Texture default_texture;
        Font default_font;

        Texture* current_texture;
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

// [iron_render_widnow]  clear and render current batch if vertex buffer is full or is end of this frame
static void RenderCurrentBatch();
// [iron_render_widnow] push/add vertex data into vertex buffer
static void PushVertexData(Texture* texture, Rect rec, V2f top_right, V2f top_left, V2f bottom_right, V2f bottom_left, Color c);


// ---------------------------------- //
//          Window Management         //
// ---------------------------------- //

Texture t;

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

    // fps
    CONTEXT.Time.current_time = 0.0f;
    CONTEXT.Time.fps_count = 0;
    CONTEXT.Time.fps = 0;

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

    
    // ResT r = LoadTextureFile(&t, "assets/kenney_animalpackredux/rabbit.png");
    // if (r != RES_SUCCESS) {
    //     return r;
    // }

#if 0
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
#endif

    // big vertex buffer for batch renderer
    CONTEXT.RenderState.vertex_buffer.vertices_count = 0;
    CONTEXT.RenderState.vertex_buffer.vertices_max = DEFAULT_MAX_RECT * VERICES_PER_RECT;
    CONTEXT.RenderState.vertex_buffer.vertices = (V2f*)malloc(DEFAULT_MAX_RECT * VERICES_PER_RECT * sizeof(V2f));

    CONTEXT.RenderState.vertex_buffer.texcoords_count = 0;
    CONTEXT.RenderState.vertex_buffer.texcoords_max = DEFAULT_MAX_RECT * TEXCOORDS_PER_RECT;
    CONTEXT.RenderState.vertex_buffer.texcoords = (V2f*)malloc(DEFAULT_MAX_RECT * TEXCOORDS_PER_RECT * sizeof(V2f));

    CONTEXT.RenderState.vertex_buffer.colors_count = 0;
    CONTEXT.RenderState.vertex_buffer.colors_max = DEFAULT_MAX_RECT * COLORS_PER_RECT;
    CONTEXT.RenderState.vertex_buffer.colors = (V4f*)malloc(DEFAULT_MAX_RECT * COLORS_PER_RECT * sizeof(V4f));

    CONTEXT.RenderState.vertex_buffer.indices_count = 0;
    CONTEXT.RenderState.vertex_buffer.indices_max = DEFAULT_MAX_RECT * INDICES_PER_RECT;
    CONTEXT.RenderState.vertex_buffer.indices = (unsigned int*)malloc(DEFAULT_MAX_RECT * INDICES_PER_RECT * sizeof(unsigned int));
    
    unsigned int offset = 0;
    for (size_t i = 0; i < (DEFAULT_MAX_RECT * INDICES_PER_RECT); i += INDICES_PER_RECT) {
        CONTEXT.RenderState.vertex_buffer.indices[i] = offset + 0;
        CONTEXT.RenderState.vertex_buffer.indices[i + 1] = offset + 1;
        CONTEXT.RenderState.vertex_buffer.indices[i + 2] = offset + 3;
        CONTEXT.RenderState.vertex_buffer.indices[i + 3] = offset + 1;
        CONTEXT.RenderState.vertex_buffer.indices[i + 4] = offset + 2;
        CONTEXT.RenderState.vertex_buffer.indices[i + 5] = offset + 3;

        offset += 4;
    }

    glGenVertexArrays(1, &CONTEXT.RenderState.vertex_buffer.vao);
    // glBindVertexArray(CONTEXT.RenderState.vertex_buffer.vao);

    glGenBuffers(4, CONTEXT.RenderState.vertex_buffer.vbo);

    glBindBuffer(GL_ARRAY_BUFFER, CONTEXT.RenderState.vertex_buffer.vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, DEFAULT_MAX_RECT * sizeof(V2f) * VERICES_PER_RECT, NULL, GL_DYNAMIC_DRAW);
    // glVertexAttribPointer(CONTEXT.RenderState.default_shader.attribs_locations[SHADER_ATTRIB_VEC2_POS], 2, GL_FLOAT, GL_FALSE, 0, 0);
    // glEnableVertexAttribArray(CONTEXT.RenderState.default_shader.attribs_locations[SHADER_ATTRIB_VEC2_POS]);

    glBindBuffer(GL_ARRAY_BUFFER, CONTEXT.RenderState.vertex_buffer.vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, DEFAULT_MAX_RECT * sizeof(V2f) * TEXCOORDS_PER_RECT, NULL, GL_DYNAMIC_DRAW);
    // glVertexAttribPointer(CONTEXT.RenderState.default_shader.attribs_locations[SHADER_ATTRIB_VEC2_TEXCOORD], 2, GL_FLOAT, GL_FALSE, 0, 0);
    // glEnableVertexAttribArray(CONTEXT.RenderState.default_shader.attribs_locations[SHADER_ATTRIB_VEC2_TEXCOORD]);

    glBindBuffer(GL_ARRAY_BUFFER, CONTEXT.RenderState.vertex_buffer.vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, DEFAULT_MAX_RECT * sizeof(V4f) * COLORS_PER_RECT, NULL, GL_DYNAMIC_DRAW);
    // glVertexAttribPointer(CONTEXT.RenderState.default_shader.attribs_locations[SHADER_ATTRIB_VEC4_COLOR], 4, GL_FLOAT, GL_FALSE, 0, 0);
    // glEnableVertexAttribArray(CONTEXT.RenderState.default_shader.attribs_locations[SHADER_ATTRIB_VEC4_COLOR]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CONTEXT.RenderState.vertex_buffer.vbo[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, DEFAULT_MAX_RECT * sizeof(unsigned int) * INDICES_PER_RECT, CONTEXT.RenderState.vertex_buffer.indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    // init draw call
    for (int i = 0; i < MAX_DRAW_CALL_PER_FRAME; i++) {
        CONTEXT.RenderState.draw_calls[i].vertices_count = 0;
    }
    CONTEXT.RenderState.draw_calls_count = 1;

	// create default texture, a pixel 1x1 white squad
    
	unsigned char white_pixel[4] = { 255, 255, 255, 255 };
	CONTEXT.RenderState.default_texture.w = 1;
	CONTEXT.RenderState.default_texture.h = 1;
	CONTEXT.RenderState.default_texture.gl_fmt = TEX_FMT_RGBA;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &CONTEXT.RenderState.default_texture.id);
	glBindTexture(GL_TEXTURE_2D, CONTEXT.RenderState.default_texture.id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white_pixel);

	glBindTexture(GL_TEXTURE_2D, 0);
    
    // set current using texture
    CONTEXT.RenderState.current_texture = &CONTEXT.RenderState.default_texture;

    // ResT load_tex = LoadTextureFile(&CONTEXT.RenderState.default_texture, "assets/kenney_animalpackredux/rabbit.png");
    // if (load_tex != RES_SUCCESS) {
    //   LogError("failed to load default texture");
    //   return load_tex;
    // }

    // load default font
    ResT load_font_res = LoadFontTTF(&CONTEXT.RenderState.default_font, 64, "assets/fonts/arial.ttf");
    if (load_font_res != RES_SUCCESS) {
        return load_font_res;
    }

    // TEST: print character data in font
/*
    LogInfo("Create default texture, id: %d", CONTEXT.RenderState.default_font.texture.id);
    for (int i = 0; i < ASCII_CHAR_COUNT; i++) {
        char c = CONTEXT.RenderState.default_font.chars[i].unicode;
        Rect rec = CONTEXT.RenderState.default_font.chars[i].rec;
        int xoffset = CONTEXT.RenderState.default_font.chars[i].xoffset;
        int yoffset = CONTEXT.RenderState.default_font.chars[i].yoffset;
        int xadvance = CONTEXT.RenderState.default_font.chars[i].xadvance;

        LogInfo("Character: %c, rect = %s, xoffset = %d, yoffset = %d, xadvance = %d", c, RectToString(rec), xoffset, yoffset, xadvance);
    }
*/

    // math... random
    srand((time_t)NULL);

    return RES_SUCCESS;
}

// [iron_render_window] release window handle and 2d renderer
void ReleaseRenderWindow(void) {

    if (CONTEXT.RenderState.vertex_buffer.vertices != NULL) {
        free(CONTEXT.RenderState.vertex_buffer.vertices);
    }

    if (CONTEXT.RenderState.vertex_buffer.texcoords != NULL) {
        free(CONTEXT.RenderState.vertex_buffer.texcoords);
    }

    if (CONTEXT.RenderState.vertex_buffer.colors != NULL) {
        free(CONTEXT.RenderState.vertex_buffer.colors);
    }

    if (CONTEXT.RenderState.vertex_buffer.indices != NULL) {
        free(CONTEXT.RenderState.vertex_buffer.indices);
    }

    glDeleteBuffers(4, CONTEXT.RenderState.vertex_buffer.vbo);
    glDeleteVertexArrays(1, &CONTEXT.RenderState.vertex_buffer.vao);
    ReleaseShader(&CONTEXT.RenderState.default_shader);
    ReleaseTexture(&CONTEXT.RenderState.default_texture);
    ReleaseFont(&CONTEXT.RenderState.default_font);

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

    // update projection matrix
    // LogInfo("Window h %d, w %d", CONTEXT.Window.h, CONTEXT.Window.w);
	CONTEXT.RenderState.projection = Mat4Ortho(0.0f, (float)CONTEXT.Window.w, (float)CONTEXT.Window.h, 0.0f, -1.0f, 1.0f);

    glfwSwapBuffers(CONTEXT.Window.handle);

    // clear frame and draw background
    glClear(GL_COLOR_BUFFER_BIT);
    V4f color = ColorToVec4f(clear_color);
    glClearColor(color.r, color.g, color.b, color.a);
}

// [iron_render_window] state of end rendering and store drawing time step, frame time step
void EndRendering() {

    RenderCurrentBatch();

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

    CONTEXT.Time.fps_time_counter += CONTEXT.Time.frame_time;
    CONTEXT.Time.fps_count += 1;
    if (CONTEXT.Time.fps_time_counter >= 1.0f) {
        CONTEXT.Time.fps_time_counter = 0.0f;
        CONTEXT.Time.fps = CONTEXT.Time.fps_count;
        CONTEXT.Time.fps_count = 0;
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

    // TODO!!

    Mat4 rotate_matrix = Mat4EulerRotate(MAT4_IDENTITY, angle);

    V2f top_right = V2F_ZERO;
    top_right.x = sz.x;
    top_right.y = sz.y;
    V4f v0 = { top_right.x, top_right.y, 0.0f, 1.0f };
    v0 = Mat4MulV4f(rotate_matrix, v0);
    top_right.x = v0.x + pos.x;
    top_right.y = v0.y + pos.y;

    // LogInfo("top right: %s", V2fToString(top_right));

    V2f top_left = V2F_ZERO;
    top_left.x = 0.0f;
    top_left.y = sz.y;
    V4f v1 = { top_left.x, top_left.y, 0.0f, 1.0f };
    v1 = Mat4MulV4f(rotate_matrix, v1);
    top_left.x = v1.x + pos.x;
    top_left.y = v1.y + pos.y;

    // LogInfo("top left: %s", V2fToString(top_left));

    V2f bottom_right = V2F_ZERO;
    bottom_right.x = sz.x;
    bottom_right.y = 0.0f;
    V4f v2 = { bottom_right.x, bottom_right.y, 0.0f, 1.0f };
    v2 = Mat4MulV4f(rotate_matrix, v2);
    bottom_right.x = v2.x + pos.x;
    bottom_right.y = v2.y + pos.y;

    // LogInfo("bottom right: %s", V2fToString(bottom_right));

    V2f bottom_left = V2F_ZERO;
    bottom_left.x = 0.0f;
    bottom_left.y = 0.0f;
    V4f v3 = { bottom_left.x, bottom_left.y, 0.0f, 1.0f };
    v3 = Mat4MulV4f(rotate_matrix, v3);
    bottom_left.x = v3.x + pos.x;
    bottom_left.y = v3.y + pos.y;

    // LogInfo("bottom left: %s", V2fToString(bottom_left));
    Rect rec = {
        0.0f, 0.0f,
        CONTEXT.RenderState.default_texture.w, CONTEXT.RenderState.default_texture.h
    };
    
    PushVertexData(&CONTEXT.RenderState.default_texture, rec, top_right, top_left, bottom_right, bottom_left, c);

#if 0
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
#endif
}

// [iron_render_window] draw line
void DrawLine(V2f p0, V2f p1, int thinkness, Color c) {
    V2f delta = V2fSub(p1, p0);
    float len = V2fLen(delta);

    float scale = thinkness / (len * 2);
    V2f p = { -scale * delta.y, scale * delta.x };

    V2f top_right = { p1.x + p.x, p1.y + p.y };
    V2f bottom_right = { p1.x - p.x, p1.y - p.y };
    V2f top_left = { p0.x + p.x, p0.y + p.y };
    V2f bottom_left = { p0.x - p.x, p0.y - p.y };

    Rect rec = {
        0.0f, 0.0f,
        CONTEXT.RenderState.default_texture.w, CONTEXT.RenderState.default_texture.h
    };
    
    PushVertexData(&CONTEXT.RenderState.default_texture, rec, top_right, top_left, bottom_right, bottom_left, c);

}

void DrawRectangleGrid(V2f pos, V2f sz, float angle, Color c) {
    Mat4 rotate_matrix = Mat4EulerRotate(MAT4_IDENTITY, angle);

    V2f top_right = V2F_ZERO;
    top_right.x = sz.x;
    top_right.y = sz.y;
    V4f v0 = { top_right.x, top_right.y, 0.0f, 1.0f };
    v0 = Mat4MulV4f(rotate_matrix, v0);
    top_right.x = v0.x + pos.x;
    top_right.y = v0.y + pos.y;

    // LogInfo("top right: %s", V2fToString(top_right));

    V2f top_left = V2F_ZERO;
    top_left.x = 0.0f;
    top_left.y = sz.y;
    V4f v1 = { top_left.x, top_left.y, 0.0f, 1.0f };
    v1 = Mat4MulV4f(rotate_matrix, v1);
    top_left.x = v1.x + pos.x;
    top_left.y = v1.y + pos.y;

    // LogInfo("top left: %s", V2fToString(top_left));

    V2f bottom_right = V2F_ZERO;
    bottom_right.x = sz.x;
    bottom_right.y = 0.0f;
    V4f v2 = { bottom_right.x, bottom_right.y, 0.0f, 1.0f };
    v2 = Mat4MulV4f(rotate_matrix, v2);
    bottom_right.x = v2.x + pos.x;
    bottom_right.y = v2.y + pos.y;

    // LogInfo("bottom right: %s", V2fToString(bottom_right));

    V2f bottom_left = V2F_ZERO;
    bottom_left.x = 0.0f;
    bottom_left.y = 0.0f;
    V4f v3 = { bottom_left.x, bottom_left.y, 0.0f, 1.0f };
    v3 = Mat4MulV4f(rotate_matrix, v3);
    bottom_left.x = v3.x + pos.x;
    bottom_left.y = v3.y + pos.y;

    // LogInfo("bottom left: %s", V2fToString(bottom_left));

    DrawLine(top_left, top_right, 2, c);
    DrawLine(top_right, bottom_right, 2, c);
    DrawLine(bottom_right, bottom_left, 2, c);
    DrawLine(bottom_left, top_left, 2, c);
}

void DrawText(const char* text, V2f pos, float sz, Color c) {
    int len = strlen(text);
	int letter = 0;
	int index = 0;

	int text_yoff = 0;
	float text_xoff = 0.0f;
	float scale = 0.0f;

	for (int i = 0; i < len; i++) {
		index = text[i] - 32;

		if (text[i] == '\n') {
			text_yoff += (CONTEXT.RenderState.default_font.base_height * sz);
			text_xoff = 0.0f;
		} else {
			if (text[i] != ' ') {

				V2f p = { 
					pos.x + text_xoff + (CONTEXT.RenderState.default_font.chars[index].xoffset * sz),
					pos.y + (float)text_yoff + (CONTEXT.RenderState.default_font.chars[index].yoffset * sz),
				};

				V2f top_right = { p.x + CONTEXT.RenderState.default_font.chars[index].rec.w * sz, p.y + CONTEXT.RenderState.default_font.chars[index].rec.h * sz };
				V2f top_left = { .x = p.x, .y = p.y + CONTEXT.RenderState.default_font.chars[index].rec.h * sz };
				V2f bottom_right = { .x = p.x + CONTEXT.RenderState.default_font.chars[index].rec.w * sz, .y = p.y };
				V2f bottom_left = { .x = p.x, .y = p.y };

				PushVertexData(&CONTEXT.RenderState.default_font.texture, CONTEXT.RenderState.default_font.chars[index].rec, top_right, top_left, bottom_right, bottom_left, c);
			}

			if (CONTEXT.RenderState.default_font.chars[index].xadvance == 0) {
				text_xoff += (CONTEXT.RenderState.default_font.chars[index].rec.w + 2.0f) * sz;
			} else {
				text_xoff += (CONTEXT.RenderState.default_font.chars[index].xadvance + 2.0f) * sz;
			}
		}
	}
}

void DrawFPS(V2f pos, float sz, Color c) {
    const char* fps_text = StringFormat("FPS: %d", CONTEXT.Time.fps);
    DrawText(fps_text, pos, sz, c);
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

// [iron_render_widnow]  clear and render current batch if vertex buffer is full or is end of this frame
static void RenderCurrentBatch() {
    // active vertex array
    glBindVertexArray(CONTEXT.RenderState.vertex_buffer.vao);

    // set vertex buffer data
    glBindBuffer(GL_ARRAY_BUFFER, CONTEXT.RenderState.vertex_buffer.vbo[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, CONTEXT.RenderState.vertex_buffer.vertices_count * sizeof(V2f), CONTEXT.RenderState.vertex_buffer.vertices);

    glBindBuffer(GL_ARRAY_BUFFER, CONTEXT.RenderState.vertex_buffer.vbo[1]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, CONTEXT.RenderState.vertex_buffer.texcoords_count * sizeof(V2f), CONTEXT.RenderState.vertex_buffer.texcoords);

    glBindBuffer(GL_ARRAY_BUFFER, CONTEXT.RenderState.vertex_buffer.vbo[2]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, CONTEXT.RenderState.vertex_buffer.colors_count * sizeof(V4f), CONTEXT.RenderState.vertex_buffer.colors);

    glUseProgram(CONTEXT.RenderState.default_shader.id);

    glUniformMatrix4fv(CONTEXT.RenderState.default_shader.attribs_locations[SHADER_ATTRIB_MAT4_MVP], 1, false, CONTEXT.RenderState.projection.unit);

    glBindBuffer(GL_ARRAY_BUFFER, CONTEXT.RenderState.vertex_buffer.vbo[0]);
    glVertexAttribPointer(CONTEXT.RenderState.default_shader.attribs_locations[SHADER_ATTRIB_VEC2_POS], 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(CONTEXT.RenderState.default_shader.attribs_locations[SHADER_ATTRIB_VEC2_POS]);

    glBindBuffer(GL_ARRAY_BUFFER, CONTEXT.RenderState.vertex_buffer.vbo[1]);
    glVertexAttribPointer(CONTEXT.RenderState.default_shader.attribs_locations[SHADER_ATTRIB_VEC2_TEXCOORD], 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(CONTEXT.RenderState.default_shader.attribs_locations[SHADER_ATTRIB_VEC2_TEXCOORD]);

    glBindBuffer(GL_ARRAY_BUFFER, CONTEXT.RenderState.vertex_buffer.vbo[2]);
    glVertexAttribPointer(CONTEXT.RenderState.default_shader.attribs_locations[SHADER_ATTRIB_VEC4_COLOR], 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(CONTEXT.RenderState.default_shader.attribs_locations[SHADER_ATTRIB_VEC4_COLOR]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CONTEXT.RenderState.vertex_buffer.vbo[3]);

    glActiveTexture(GL_TEXTURE0);

    // draw!!
    size_t vertex_offset = 0;
    for (size_t i = 0; i < CONTEXT.RenderState.draw_calls_count; i++) {
        glBindTexture(GL_TEXTURE_2D, CONTEXT.RenderState.draw_calls[i].texture_id);
        glDrawElements(GL_TRIANGLES, (CONTEXT.RenderState.draw_calls[i].vertices_count / VERICES_PER_RECT * INDICES_PER_RECT), GL_UNSIGNED_INT, (const GLvoid*)(sizeof(unsigned int) * vertex_offset / VERICES_PER_RECT * INDICES_PER_RECT));
        vertex_offset += CONTEXT.RenderState.draw_calls[i].vertices_count;
        // LogInfo("Current draw %d, vertices count %d", (i + 1), CONTEXT.RenderState.draw_calls[i].vertices_count);
    }

    glBindVertexArray(0);

    // reset vertex buffer and draw calls
    CONTEXT.RenderState.vertex_buffer.vertices_count = 0;
    CONTEXT.RenderState.vertex_buffer.texcoords_count = 0;
    CONTEXT.RenderState.vertex_buffer.colors_count = 0;
    CONTEXT.RenderState.vertex_buffer.indices_count = 0;

    for (int i = 0; i < MAX_DRAW_CALL_PER_FRAME; i++) {
        CONTEXT.RenderState.draw_calls[i].vertices_count = 0;
    }
    CONTEXT.RenderState.draw_calls_count = 1;
}

// [iron_render_widnow] push/add vertex data into vertex buffer
static void PushVertexData(Texture* texture, Rect rec, V2f top_right, V2f top_left, V2f bottom_right, V2f bottom_left, Color c) {

    if (CONTEXT.RenderState.vertex_buffer.vertices_count >= (DEFAULT_MAX_RECT * VERICES_PER_RECT)) { // check vertex buffer is full
        LogInfo("Vertex buffer capacity is max");
        RenderCurrentBatch();
    }

    // check texture id from current draw call and vertex buffer
    if (CONTEXT.RenderState.draw_calls[CONTEXT.RenderState.draw_calls_count - 1].texture_id != texture->id) {

        // check is there any shapes are waiting to draw
        if (CONTEXT.RenderState.draw_calls[CONTEXT.RenderState.draw_calls_count - 1].vertices_count > 0) {
            CONTEXT.RenderState.draw_calls_count += 1;
        }

        if (CONTEXT.RenderState.draw_calls_count >= MAX_DRAW_CALL_PER_FRAME) {
            RenderCurrentBatch();
        }

        CONTEXT.RenderState.draw_calls[CONTEXT.RenderState.draw_calls_count - 1].texture_id = texture->id;
        CONTEXT.RenderState.draw_calls[CONTEXT.RenderState.draw_calls_count - 1].vertices_count = 0;
    }

    // clock-wise
    V4f color_v4 = ColorToVec4f(c);

    // top right
    CONTEXT.RenderState.vertex_buffer.vertices[CONTEXT.RenderState.vertex_buffer.vertices_count].x = top_right.x;
	CONTEXT.RenderState.vertex_buffer.vertices[CONTEXT.RenderState.vertex_buffer.vertices_count].y = top_right.y;
	CONTEXT.RenderState.vertex_buffer.vertices_count += 1;

	CONTEXT.RenderState.vertex_buffer.texcoords[CONTEXT.RenderState.vertex_buffer.texcoords_count].x = (rec.x + rec.w) / texture->w;
	CONTEXT.RenderState.vertex_buffer.texcoords[CONTEXT.RenderState.vertex_buffer.texcoords_count].y = (rec.y + rec.h) / texture->h;
	CONTEXT.RenderState.vertex_buffer.texcoords_count += 1;

	CONTEXT.RenderState.vertex_buffer.colors[CONTEXT.RenderState.vertex_buffer.colors_count].r = color_v4.r;
	CONTEXT.RenderState.vertex_buffer.colors[CONTEXT.RenderState.vertex_buffer.colors_count].g = color_v4.g;
	CONTEXT.RenderState.vertex_buffer.colors[CONTEXT.RenderState.vertex_buffer.colors_count].b = color_v4.b;
	CONTEXT.RenderState.vertex_buffer.colors[CONTEXT.RenderState.vertex_buffer.colors_count].a = color_v4.a;
	CONTEXT.RenderState.vertex_buffer.colors_count += 1;

	CONTEXT.RenderState.draw_calls[CONTEXT.RenderState.draw_calls_count - 1].vertices_count += 1;

    // bottom right
    CONTEXT.RenderState.vertex_buffer.vertices[CONTEXT.RenderState.vertex_buffer.vertices_count].x = bottom_right.x;
	CONTEXT.RenderState.vertex_buffer.vertices[CONTEXT.RenderState.vertex_buffer.vertices_count].y = bottom_right.y;
	CONTEXT.RenderState.vertex_buffer.vertices_count += 1;

	CONTEXT.RenderState.vertex_buffer.texcoords[CONTEXT.RenderState.vertex_buffer.texcoords_count].x = (rec.x + rec.w) / texture->w;
	CONTEXT.RenderState.vertex_buffer.texcoords[CONTEXT.RenderState.vertex_buffer.texcoords_count].y = rec.y / texture->h;
	CONTEXT.RenderState.vertex_buffer.texcoords_count += 1;

	CONTEXT.RenderState.vertex_buffer.colors[CONTEXT.RenderState.vertex_buffer.colors_count].r = color_v4.r;
	CONTEXT.RenderState.vertex_buffer.colors[CONTEXT.RenderState.vertex_buffer.colors_count].g = color_v4.g;
	CONTEXT.RenderState.vertex_buffer.colors[CONTEXT.RenderState.vertex_buffer.colors_count].b = color_v4.b;
	CONTEXT.RenderState.vertex_buffer.colors[CONTEXT.RenderState.vertex_buffer.colors_count].a = color_v4.a;
	CONTEXT.RenderState.vertex_buffer.colors_count += 1;

	CONTEXT.RenderState.draw_calls[CONTEXT.RenderState.draw_calls_count - 1].vertices_count += 1;

    // bottom left
    CONTEXT.RenderState.vertex_buffer.vertices[CONTEXT.RenderState.vertex_buffer.vertices_count].x = bottom_left.x;
	CONTEXT.RenderState.vertex_buffer.vertices[CONTEXT.RenderState.vertex_buffer.vertices_count].y = bottom_left.y;
	CONTEXT.RenderState.vertex_buffer.vertices_count += 1;

	CONTEXT.RenderState.vertex_buffer.texcoords[CONTEXT.RenderState.vertex_buffer.texcoords_count].x = rec.x / texture->w;
	CONTEXT.RenderState.vertex_buffer.texcoords[CONTEXT.RenderState.vertex_buffer.texcoords_count].y = rec.y / texture->h;
	CONTEXT.RenderState.vertex_buffer.texcoords_count += 1;

	CONTEXT.RenderState.vertex_buffer.colors[CONTEXT.RenderState.vertex_buffer.colors_count].r = color_v4.r;
	CONTEXT.RenderState.vertex_buffer.colors[CONTEXT.RenderState.vertex_buffer.colors_count].g = color_v4.g;
	CONTEXT.RenderState.vertex_buffer.colors[CONTEXT.RenderState.vertex_buffer.colors_count].b = color_v4.b;
	CONTEXT.RenderState.vertex_buffer.colors[CONTEXT.RenderState.vertex_buffer.colors_count].a = color_v4.a;
	CONTEXT.RenderState.vertex_buffer.colors_count += 1;

	CONTEXT.RenderState.draw_calls[CONTEXT.RenderState.draw_calls_count - 1].vertices_count += 1;

    // top left
    CONTEXT.RenderState.vertex_buffer.vertices[CONTEXT.RenderState.vertex_buffer.vertices_count].x = top_left.x;
	CONTEXT.RenderState.vertex_buffer.vertices[CONTEXT.RenderState.vertex_buffer.vertices_count].y = top_left.y;
	CONTEXT.RenderState.vertex_buffer.vertices_count += 1;

	CONTEXT.RenderState.vertex_buffer.texcoords[CONTEXT.RenderState.vertex_buffer.texcoords_count].x = rec.x / texture->w;
	CONTEXT.RenderState.vertex_buffer.texcoords[CONTEXT.RenderState.vertex_buffer.texcoords_count].y = (rec.y + rec.h) / texture->h;
	CONTEXT.RenderState.vertex_buffer.texcoords_count += 1;

	CONTEXT.RenderState.vertex_buffer.colors[CONTEXT.RenderState.vertex_buffer.colors_count].r = color_v4.r;
	CONTEXT.RenderState.vertex_buffer.colors[CONTEXT.RenderState.vertex_buffer.colors_count].g = color_v4.g;
	CONTEXT.RenderState.vertex_buffer.colors[CONTEXT.RenderState.vertex_buffer.colors_count].b = color_v4.b;
	CONTEXT.RenderState.vertex_buffer.colors[CONTEXT.RenderState.vertex_buffer.colors_count].a = color_v4.a;
	CONTEXT.RenderState.vertex_buffer.colors_count += 1;

	CONTEXT.RenderState.draw_calls[CONTEXT.RenderState.draw_calls_count - 1].vertices_count += 1;

    // indices
    CONTEXT.RenderState.vertex_buffer.indices_count += 6;

#if 0
    if (CONTEXT.RenderState.draw_calls[CONTEXT.RenderState.draw_calls_count - 1].vertices_count >= (DEFAULT_MAX_RECT * VERICES_PER_RECT)) {
         
        LogInfo("Reached max count");

        // draw call array is full
        if (CONTEXT.RenderState.draw_calls_count >= MAX_DRAW_CALL_PER_FRAME) {
            RenderCurrentBatch();
            CONTEXT.RenderState.draw_calls_count = 1;
        }

        CONTEXT.RenderState.draw_calls_count += 1;
        CONTEXT.RenderState.draw_calls[CONTEXT.RenderState.draw_calls_count - 1].vertices_count = 0;
    }
#endif
}
