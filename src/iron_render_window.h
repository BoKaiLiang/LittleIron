#ifndef _IRON_RENDER_WINDOW_H_
#define _IRON_RENDER_WINDOW_H_

#include <stdbool.h>

#include "iron_types.h"

// ---------------------------------- //
//          Window Management         //
// ---------------------------------- //

// [iron_render_window] create window handle and 2d renderer
ResT CreateRenderWindow(int w, int h, const char* title, int fps);

// [iron_render_window] release window handle and 2d renderer
void ReleaseRenderWindow(void);

// [iron_render_window] check is widnow running
bool IsWindowRunning(void);

// [iron_render_window] release window handle and 2d renderer
void CloseRenderWindow(void);

// [iron_render_window] state of state rendering and store logic update time step
void BeginRednering(Color clear_color);

// [iron_render_window] state of end rendering and store drawing time step, frame time step
void EndRendering();

// ---------------------------------- //
//              Input                 //
// ---------------------------------- //

// [iron_render_window] check is key pressed
bool IsKeyPressed(KeyCode keycode);

// [iron_render_window] check is key released after pressed
bool IsKeyReleased(KeyCode keycode);

// [iron_render_window] check is key down
bool IsKeyDown(KeyCode keycode);

// [iron_render_window] check is key up
bool IsKeyUp(KeyCode keycode);

// [iron_render_window] check is mouse button pressed
bool IsMouseButtonPressed(MouseButton mouse_button);

// [iron_render_window] check is mouse button released after pressed
bool IsMouseButtonReleased(MouseButton mouse_button);

// [iron_render_window] get cursor position
V2f GetMousePosition();

// [iron_render_window] get mouse scroll move
float GetScrollYOffset();

// ---------------------------------- //
//                Time                //
// ---------------------------------- //

// [iron_render_window] get game loop time
float GetTime();

// [iron_render_window] get delta time(frame time)
float GetDeltaTime();

// ---------------------------------- //
//               Render               //
// ---------------------------------- //

// [iron_render_window] draw rectangle
void DrawRectangle(V2f pos, V2f sz, float angle, Color c);

// [iron_render_window] draw line
void DrawLine(V2f p0, V2f p1, int thinkness, Color c);

void DrawRectangleGrid(V2f pos, V2f sz, float angle, Color c);

void DrawText(const char* text, V2f pos, float sz, Color c);

void DrawFPS(V2f pos, float sz, Color c);

#endif // _IRON_RENDER_WINDOW_H_