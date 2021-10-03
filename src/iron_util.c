#include "iron_util.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static const char* LOG_LABELS[MAX_LOG_TYPE] = {
    [LOG_INFO] = "[INFO]",
    [LOG_WARN] = "[WARN]",
    [LOG_ERROR] = "[ERROR]",
}; 

#define MAX_BUFFERS_COUNT 64
#define MAX_BUFFFERS_CHAR_LEN 1024

static char STR_BUFFERS[MAX_BUFFERS_COUNT][MAX_BUFFFERS_CHAR_LEN] = { 0 };
static int BUFFER_INDEX = 0;

// [iron_util] base format function to print the log
void LogMessage(LogT log_type, const char* file_name, int line, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    fprintf(stdout, "%s %s:%d ", LOG_LABELS[log_type], file_name, line);
    vfprintf(stdout, fmt, args);
    fprintf(stdout, "\n");

    va_end(args);
}

// [iron_util] covert color to vec4f
V4f ColorToVec4f(Color c) {
    return (V4f) {
        c.r / 255.0f,
        c.g / 255.0f,
        c.b / 255.0f,
        c.a / 255.0f,
    };
}

// [iron_util] V2f to string
const char* V2fToString(V2f v) {
    char* buffer = STR_BUFFERS[BUFFER_INDEX];
    memset(buffer, 0, MAX_BUFFFERS_CHAR_LEN);
    
    sprintf_s(buffer, MAX_BUFFFERS_CHAR_LEN, "( %.4f, %.4f )", v.x, v.y);

    BUFFER_INDEX += 1;
    if (BUFFER_INDEX >= MAX_BUFFERS_COUNT) {
        BUFFER_INDEX = 0;
    }

    return buffer;
}

// [iron_util] V4f to string
const char* V4fToString(V4f v) {
    char* buffer = STR_BUFFERS[BUFFER_INDEX];
    memset(buffer, 0, MAX_BUFFFERS_CHAR_LEN);
    
    sprintf_s(buffer, MAX_BUFFFERS_CHAR_LEN, "( %.4f, %.4f, %.4f, %.4f )", v.x, v.y, v.z, v.w);

    BUFFER_INDEX += 1;
    if (BUFFER_INDEX >= MAX_BUFFERS_COUNT) {
        BUFFER_INDEX = 0;
    }

    return buffer;
}

// [iron_util] Color to string
const char* ColorToString(Color c) {
    char* buffer = STR_BUFFERS[BUFFER_INDEX];
    memset(buffer, 0, MAX_BUFFFERS_CHAR_LEN);
    
    sprintf_s(buffer, MAX_BUFFFERS_CHAR_LEN, "[ r: %d, g: %d, b: %d, a: %d ]", c.r, c.g, c.b, c.a);

    BUFFER_INDEX += 1;
    if (BUFFER_INDEX >= MAX_BUFFERS_COUNT) {
        BUFFER_INDEX = 0;
    }

    return buffer;
}

// [iron_util] Mat4 to string
const char* Mat4ToString(Mat4 m) {
    char* buffer = STR_BUFFERS[BUFFER_INDEX];
    memset(buffer, 0, MAX_BUFFFERS_CHAR_LEN);

    sprintf_s(buffer, MAX_BUFFFERS_CHAR_LEN, 
            "\n\t\t\t\t|%.4f, %.4f, %.4f, %.4f|\n\t\t\t\t|%.4f, %.4f, %.4f, %.4f|\n\t\t\t\t|%.4f, %.4f, %.4f, %.4f|\n\t\t\t\t|%.4f, %.4f, %.4f, %.4f|\n", 
            m.unit[0], m.unit[4], m.unit[8], m.unit[12],
            m.unit[1], m.unit[5], m.unit[9], m.unit[13],
            m.unit[2], m.unit[6], m.unit[10], m.unit[14],
            m.unit[3], m.unit[7], m.unit[11], m.unit[15]
    );

    BUFFER_INDEX += 1;
    if (BUFFER_INDEX >= MAX_BUFFERS_COUNT) {
        BUFFER_INDEX = 0;
    }

    return buffer;
}

// [iron_util] Rect to string
const char* RectToString(Rect r) {
    char* buffer = STR_BUFFERS[BUFFER_INDEX];
    memset(buffer, 0, MAX_BUFFFERS_CHAR_LEN);

    sprintf_s(buffer, MAX_BUFFFERS_CHAR_LEN, "( x = %.4f, y = %.4f, width = %.4f, height = %.4f )", r.x, r.y, r.w, r.h);

    BUFFER_INDEX += 1;
    if (BUFFER_INDEX >= MAX_BUFFERS_COUNT) {
        BUFFER_INDEX = 0;
    }

    return buffer;
}