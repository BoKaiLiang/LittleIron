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

#endif // _IRON_WINDOW_H_