#ifndef _IRON_TYPES_H_
#define _IRON_TYPES_H_

#include <stdint.h>

#include "glad/gl.h"
#include "GLFW/glfw3.h"

#define ASCII_CHAR_COUNT 95

#define COLOR_NONE ((Color){ 0, 0, 0, 0 })
#define COLOR_WHITE ((Color){ 255, 255, 255, 255 })
#define COLOR_BLACK ((Color){ 0, 0, 0, 255 })
#define COLOR_GRAY ((Color){ 128, 128, 128, 255 })
#define COLOR_RED ((Color){ 255, 0, 0, 255 })
#define COLOR_PINK ((Color){ 255, 102, 178, 255 })
#define COLOR_ORANGE ((Color){ 255, 128, 0, 255 })
#define COLOR_YELLOW ((Color){ 255, 255, 0, 255 })
#define COLOR_GREEN ((Color){ 0, 255, 0, 255 })
#define COLOR_BLUE ((Color){ 0, 0, 255, 255 })
#define COLOR_CYAN ((Color){ 0, 255, 255, 255 })
#define COLOR_PURPLE ((Color){ 106, 13, 173, 255 })

#define V2F_ZERO ((V2f){ 0.0f, 0.0f})
#define V2F_ONE ((V2f){ 1.0f, 1.0f})
#define V2F_X ((V2f){ 1.0f, 0.0f})
#define V2F_Y ((V2f){ 0.0f, 1.0f})

#define V4F_ZERO ((V4f){ 0.0f, 0.0f, 0.0f, 0.0f })
#define V4F_ONE ((V4f){ 1.0f, 1.0f, 1.0f, 1.0f })
#define V4F_X ((V4f){ 1.0f, 0.0f, 0.0f, 0.0f })
#define V4F_Y ((V4f){ 0.0f, 1.0f, 0.0f, 0.0f})
#define V4F_Z ((V4f){ 0.0f, 0.0f, 1.0f, 0.0f})
#define V4F_W ((V4f){ 0.0f, 0.0f, 0.0f, 1.0f})

#define MAT4_IDENTITY ((Mat4){1.0f, 0.0f, 0.0f, 0.0f,		\
							  0.0f, 1.0f, 0.0f, 0.0f,		\
							  0.0f, 0.0f, 1.0f, 0.0f,		\
							  0.0f, 0.0f, 0.0f, 1.0f})

typedef enum LogType {
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,

    MAX_LOG_TYPE,
} LogT;

typedef enum ResultType {
    RES_SUCCESS                     = 0,

    // error for create window
    RES_ERROR_CREATE_WINDOW         = 1,

    // error for start up glad
    RES_ERROR_LOAD_GLAD             = 2,

    // error for loading assets
    RES_ERROR_LOAD_VERTEX_SHADER        = 3,
    RES_ERROR_LOAD_FRAGMENT_SHADER      = 4,
    RES_ERROR_CREATE_SHADER_PROGRAM     = 5,
    RES_ERROR_GET_SHADER_ATTRIBUTE      = 6,
    RES_ERROR_GET_SHADER_UNIFORM        = 6,
    
    RES_ERROR_LOAD_IMAGE_FILE           = 7,

    RES_ERROR_LOAD_TTF_FILE             = 8,
    RES_ERROR_CREATE_STB_TTF_OBJ        = 9,
    RES_ERROR_CREATE_FONT_TEXTURE       = 10,
} ResT;

typedef struct Vec2f {
    float x, y;
} V2f;

typedef struct Color {
    uint8_t r, g, b, a; // uint8_t == unsigned char
} Color;

typedef struct Vec4f {
    union {
        struct { float x, y, z, w; };
        struct { float r, g, b, a; };
    };
} V4f;

typedef struct Mat4x4 {
    union {
        struct { V4f v0, v1, v2, v3; };
        float ary2d[4][4];
        float unit[16];
    };
} Mat4;

typedef struct Rect {
    union {
        struct {
            float x, y;
            float w, h;
        };

        struct {
            V2f pos;
            V2f sz;
        };
        
    };
} Rect;

typedef enum ShaderAttribType {
    SHADER_ATTRIB_VEC2_POS,
    SHADER_ATTRIB_VEC2_TEXCOORD,
    SHADER_ATTRIB_VEC4_COLOR,
    SHADER_ATTRIB_SAMPLER2D_TEXTURE,
    SHADER_ATTRIB_MAT4_MVP,

    MAX_SHADER_ATTTRIBS,
} ShaderAttribT;

typedef struct Shader {
    unsigned int id;
    int attribs_locations[MAX_SHADER_ATTTRIBS];
} Shader;

typedef enum ImageFmt {
    IMG_FMT_WHITE_BLACK = GL_R8,
    IMG_FMT_GRAYSACLE   = GL_RG8,
    IMG_FMT_RGB         = GL_RGB8,
    IMG_FMT_RGBA        = GL_RGBA8,
} ImageFmt;

typedef enum GL_TextureFmt{
    TEX_FMT_WHITE_BLACK = GL_RED,
    TEX_FMT_GRAYSCALE   = GL_RG,
    TEX_FMT_RGB         = GL_RGB,
    TEX_FMT_RGBA        = GL_RGBA,
} TextureFmt;

typedef struct Image {
    unsigned char* data;
    int w, h;
    ImageFmt src_fmt;
} Image;

typedef struct Texture {
    unsigned int id;
    int w, h;
    TextureFmt gl_fmt;
} Texture;

typedef struct CharInfo {
    int unicode;
    unsigned char* data;
    Rect rec;
    int xoffset, yoffset;
    int xadvance;
} CharInfo;

typedef struct Font {
    Texture texture;
    int base_height;
    CharInfo chars[ASCII_CHAR_COUNT];
} Font;

typedef enum KeyCode {
    /* Printable keys */
    KEY_SPACE             = 32,
    KEY_APOSTROPHE        = 39,  /* ' */
    KEY_COMMA             = 44,  /* , */
    KEY_MINUS             = 45,  /* - */
    KEY_PERIOD            = 46,  /* . */
    KEY_SLASH             = 47,  /* / */
    KEY_0                 = 48,
    KEY_1                 = 49,
    KEY_2                 = 50,
    KEY_3                 = 51,
    KEY_4                 = 52,
    KEY_5                 = 53,
    KEY_6                 = 54,
    KEY_7                 = 55,
    KEY_8                 = 56,
    KEY_9                 = 57,
    KEY_SEMICOLON         = 59, /* ; */
    KEY_EQUAL             = 61,  /* = */
    KEY_A                 = 65,
    KEY_B                 = 66,
    KEY_C                 = 67,
    KEY_D                 = 68,
    KEY_E                 = 69,
    KEY_F                 = 70,
    KEY_G                 = 71,
    KEY_H                 = 72,
    KEY_I                 = 73,
    KEY_J                 = 74,
    KEY_K                 = 75,
    KEY_L                 = 76,
    KEY_M                 = 77,
    KEY_N                 = 78,
    KEY_O                 = 79,
    KEY_P                 = 80,
    KEY_Q                 = 81,
    KEY_R                 = 82,
    KEY_S                 = 83,
    KEY_T                 = 84,
    KEY_U                 = 85,
    KEY_V                 = 86,
    KEY_W                 = 87,
    KEY_X                 = 88,
    KEY_Y                 = 89,
    KEY_Z                 = 90,
    KEY_LEFT_BRACKET      = 91, /* [ */
    KEY_BACKSLASH         = 92,  /* \ */
    KEY_RIGHT_BRACKET     = 93,  /* ] */
    KEY_GRAVE_ACCENT      = 96,  /* ` */
    KEY_WORLD_1           = 161, /* non-US #1 */
    KEY_WORLD_2           = 162, /* non-US #2 */

    /* Function keys */
    KEY_ESCAPE            = 256,
    KEY_ENTER             = 257,
    KEY_TAB               = 258,
    KEY_BACKSPACE         = 259,
    KEY_INSERT            = 260,
    KEY_DELETE            = 261,
    KEY_RIGHT             = 262,
    KEY_LEFT              = 263,
    KEY_DOWN              = 264,
    KEY_UP                = 265,
    KEY_PAGE_UP           = 266,
    KEY_PAGE_DOWN         = 267,
    KEY_HOME              = 268,
    KEY_END               = 269,
    KEY_CAPS_LOCK         = 280,
    KEY_SCROLL_LOCK       = 281,
    KEY_NUM_LOCK          = 282,
    KEY_PRINT_SCREEN      = 283,
    KEY_PAUSE             = 284,
    KEY_F1                = 290,
    KEY_F2                = 291,
    KEY_F3                = 292,
    KEY_F4                = 293,
    KEY_F5                = 294,
    KEY_F6                = 295,
    KEY_F7                = 296,
    KEY_F8                = 297,
    KEY_F9                = 298,
    KEY_F10               = 299,
    KEY_F11               = 300,
    KEY_F12               = 301,
    KEY_F13               = 302,
    KEY_F14               = 303,
    KEY_F15               = 304,
    KEY_F16               = 305,
    KEY_F17               = 306,
    KEY_F18               = 307,
    KEY_F19               = 308,
    KEY_F20               = 309,
    KEY_F21               = 310,
    KEY_F22               = 311,
    KEY_F23               = 312,
    KEY_F24               = 313,
    KEY_F25               = 314,
    KEY_KP_0              = 320,
    KEY_KP_1              = 321,
    KEY_KP_2              = 322,
    KEY_KP_3              = 323,
    KEY_KP_4              = 324,
    KEY_KP_5              = 325,
    KEY_KP_6              = 326,
    KEY_KP_7              = 327,
    KEY_KP_8              = 328,
    KEY_KP_9              = 329,
    KEY_KP_DECIMAL        = 330,
    KEY_KP_DIVIDE         = 331,
    KEY_KP_MULTIPLY       = 332,
    KEY_KP_SUBTRACT       = 333,
    KEY_KP_ADD            = 334,
    KEY_KP_ENTER          = 335,
    KEY_KP_EQUAL          = 336,
    KEY_LEFT_SHIFT        = 340,
    KEY_LEFT_CONTROL      = 341,
    KEY_LEFT_ALT          = 342,
    KEY_LEFT_SUPER        = 343,
    KEY_RIGHT_SHIFT       = 344,
    KEY_RIGHT_CONTROL     = 345,
    KEY_RIGHT_ALT         = 346,
    KEY_RIGHT_SUPER       = 347,
    KEY_MENU              = 348,

    MAX_KEY_CODE,
} KeyCode;

typedef enum MouseButton {
    MOUSE_LEFT_BUTTON   =  0,
    MOUSE_RIGHT_BUTTON  =  1,
    MOUSE_MIDDLE_BUTTON =  2,

    MAX_MOUSE_BUTTON,
} MouseButton;

typedef struct Entity {
    V2f pos;
    V2f sz;
    V2f dir;
    Rect rec;
    V2f center_pos;
} Entity;

#endif // _IRON_TYPES_H_