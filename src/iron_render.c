#include "iron_render.h"

#include <stdio.h>

#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"

#include "GLFW/glfw3.h"

#include "iron_types.h"
#include "iron_asset.h"
#include "iron_util.h"

static const char* DEFAULT_2D_VERTEX_SHADER_CODE = "#version 330 core\n"
"in vec2 _Pos;\n"
"in vec2 _Texcoords;\n"
"out vec2 Texcoords;\n"
"void main() {\n"
"   gl_Position = vec4(_Pos, 0.0, 1.0);\n"
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

static struct {
	unsigned vbo, vao, ibo;

    Shader default_shader;
	Texture default_texture;
} RENDER_2D_CONTEXT;

//-------------vertices data------------- 
static const float VERTICES[] = {
		0.5f,  0.5f, 1.0f, 1.0f,	// top right
     	0.5f, -0.5f, 1.0f, 0.0f, 	// bottom right
    	-0.5f, -0.5f, 0.0f, 0.0f, 	// bottom left
    	-0.5f,  0.5f, 0.0f, 1.0f	// top left 
	};

static const unsigned int INDICES[] = {
	0, 1, 3,   
	1, 2, 3
};
// --------------------------------------

// [iron_render] create/initialize 2d renderer
ResT CreateRenderer() {
    // Init glad
    if (!gladLoadGL(glfwGetProcAddress)) {
		LogError("Failed to load glad.");
        return RES_ERROR_LOAD_GLAD;
    }

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Load default shader
    ResT load_shader_res = LoadShaderCode(&RENDER_2D_CONTEXT.default_shader,
        DEFAULT_2D_VERTEX_SHADER_CODE, 
        DEFAULT_2D_FRAGMENT_SHADER_CODE);

    if (load_shader_res != RES_SUCCESS) {
        LogError("Failed to load default shader");
        return load_shader_res;
    }

	// init vertex buffer object, vertex array object and index buffer object
	glGenVertexArrays(1, &RENDER_2D_CONTEXT.vao);
	glBindVertexArray(RENDER_2D_CONTEXT.vao);

	glGenBuffers(1, &RENDER_2D_CONTEXT.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, RENDER_2D_CONTEXT.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

	glGenBuffers(1, &RENDER_2D_CONTEXT.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RENDER_2D_CONTEXT.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INDICES), INDICES, GL_STATIC_DRAW);

	// set position attribute
	glVertexAttribPointer(RENDER_2D_CONTEXT.default_shader.attribs_locations[SHADER_ATTRIB_VEC2_POS], 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(RENDER_2D_CONTEXT.default_shader.attribs_locations[SHADER_ATTRIB_VEC2_POS]);

	// set texture coordinate attribute
	glVertexAttribPointer(RENDER_2D_CONTEXT.default_shader.attribs_locations[SHADER_ATTRIB_VEC2_TEXCOORD], 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(RENDER_2D_CONTEXT.default_shader.attribs_locations[SHADER_ATTRIB_VEC2_TEXCOORD]);

	glBindVertexArray(0);

	// create default texture, a pixel 1x1 white squad
	unsigned char white_pixel[4] = { 255, 255, 255, 255 };

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &RENDER_2D_CONTEXT.default_texture.id);
	glBindTexture(GL_TEXTURE_2D, RENDER_2D_CONTEXT.default_texture.id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white_pixel);

	glBindTexture(GL_TEXTURE_2D, 0);


    return RES_SUCCESS;    
}

// [iron_render] create/initialize 2d renderer
void ReleaseRenderer() {
    ReleaseShader(&RENDER_2D_CONTEXT.default_shader);
}

// [iron_render] draw a simple rectangle...
void DrawRectangle(Color c) {

	glBindVertexArray(RENDER_2D_CONTEXT.vao);

	glUseProgram(RENDER_2D_CONTEXT.default_shader.id);

	V4f v = ColorToVec4f(c);
	glUniform4f(RENDER_2D_CONTEXT.default_shader.attribs_locations[SHADER_ATTRIB_VEC4_COLOR], v.r, v.g, v.b, v.a);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, RENDER_2D_CONTEXT.default_texture.id);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

// [iron_render] TEST: draw texture
void DrawTexture(Texture texture, Color c) {
	glBindVertexArray(RENDER_2D_CONTEXT.vao);

	glUseProgram(RENDER_2D_CONTEXT.default_shader.id);

	// set shader color
	V4f v = ColorToVec4f(c);
	glUniform4f(RENDER_2D_CONTEXT.default_shader.attribs_locations[SHADER_ATTRIB_VEC4_COLOR], v.r, v.g, v.b, v.a);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.id);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}