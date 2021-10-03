#ifndef _IRON_UTIL_H_
#define _IRON_UTIL_H_

#include "iron_types.h"

#define LogInfo(...) LogMessage(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__);
#define LogWarning(...) LogMessage(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__);
#define LogError(...) LogMessage(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__);

// [iron_util] base format function to print the log
void LogMessage(LogT log_type, const char* file_name, int line, const char* fmt, ...);

// [iron_util] covert color to vec4f
V4f ColorToVec4f(Color c);

// [iron_util] V2f to string
const char* V2fToString(V2f v);

// [iron_util] V4f to string
const char* V4fToString(V4f v);

// [iron_util] Color to string
const char* ColorToString(Color c);

// [iron_util] Mat4 to string
const char* Mat4ToString(Mat4 m);

// [iron_util] Rect to string
const char* RectToString(Rect r);

#endif // _IRON_UTIL_H_