#ifndef _IRON_WINDOW_H_
#define _IRON_WINDOW_H_

#include "iron_types.h"

// [iron_window] create window
ResT CreateWindow(int w, int h, const char* title);

// [iron_window] release window
void ReleaseWindow(void);

// [iron_window] check is window should be close or not
int IsWindowRunning(void);

// [iron_window] close window
void CloseWindow(void);

// [iron_window] first state in game loop, refresh the scene
void StartScene(Color clear_color);

// [iron_window] last state in game loop, store scene data in this frame
void EndScene();

// [iron_window] check is key pressed
int IsKeyPressed(KeyCode keycode);

// [iron_window] check is key released after pressed
int IsKeyReleased(KeyCode keycode);

// [iron_window] check is key up
int IsKeyDown(KeyCode keycode);

// [iron_window] check is key down
int IsKeyUp(KeyCode keycode);

// [iron_window] check is mouse button pressed
int IsMouseButtonPressed(MouseButton mouse_button);

// [iron_window] check is mouse button released
int IsMouseButtonReleased(MouseButton mouse_button);

// [iron_window] get mouse cursor position
V2f GetMousePosition();

// [iron_window] get mouse scroll roll
float GetScrollYOffset();

#endif // _IRON_WINDOW_H_