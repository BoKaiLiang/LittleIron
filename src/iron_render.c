#include "iron_render.h"

#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"

#include "GLFW/glfw3.h"

ResT CreateRenderer() {
    // Init glad
    if (!gladLoadGL(glfwGetProcAddress)) {
        printf("Failed to load glad.");
        return RES_ERROR_LOAD_GLAD;
    }

    return RES_SUCCESS;    
}

void ReleaseRenderer() {

}