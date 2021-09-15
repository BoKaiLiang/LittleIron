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
    Shader default_shader;
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
        printf("Failed to load glad.");
        return RES_ERROR_LOAD_GLAD;
    }

    // Load default shader
    ResT load_shader_res = LoadShaderCode(&RENDER_2D_CONTEXT.default_shader,
        DEFAULT_2D_VERTEX_SHADER_CODE, 
        DEFAULT_2D_FRAGMENT_SHADER_CODE);

    if (load_shader_res != RES_SUCCESS) {
        printf("Failed to load default shader\n");
        return load_shader_res;
    }

    return RES_SUCCESS;    
}

// [iron_render] create/initialize 2d renderer
void ReleaseRenderer() {
    ReleaseShader(&RENDER_2D_CONTEXT.default_shader);
}

// [iron_render] draw a simple rectangle...
void DrawFirstRectangle(Color c) {

	unsigned int vbo, vao, ibo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INDICES), INDICES, GL_STATIC_DRAW);

	glVertexAttribPointer(RENDER_2D_CONTEXT.default_shader.attribs_locations[SHADER_ATTRIB_VEC2_POS], 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(RENDER_2D_CONTEXT.default_shader.attribs_locations[SHADER_ATTRIB_VEC2_POS]);

	glUseProgram(RENDER_2D_CONTEXT.default_shader.id);


	V4f v = ColorToVec4f(c);
	glUniform4f(RENDER_2D_CONTEXT.default_shader.attribs_locations[SHADER_ATTRIB_VEC4_COLOR], v.r, v.g, v.b, v.a);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

// [iron_render] TEST: draw texture
void DrawFirstTexture(Texture* texture, Color c) {
	unsigned int vbo, vao, ibo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INDICES), INDICES, GL_STATIC_DRAW);

	// set position attribute
	glVertexAttribPointer(RENDER_2D_CONTEXT.default_shader.attribs_locations[SHADER_ATTRIB_VEC2_POS], 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(RENDER_2D_CONTEXT.default_shader.attribs_locations[SHADER_ATTRIB_VEC2_POS]);

	// set texture coordinate attribute
	glVertexAttribPointer(RENDER_2D_CONTEXT.default_shader.attribs_locations[SHADER_ATTRIB_VEC2_TEXCOORD], 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(RENDER_2D_CONTEXT.default_shader.attribs_locations[SHADER_ATTRIB_VEC2_TEXCOORD]);

	glUseProgram(RENDER_2D_CONTEXT.default_shader.id);

	// set shader color
	V4f v = ColorToVec4f(c);
	glUniform4f(RENDER_2D_CONTEXT.default_shader.attribs_locations[SHADER_ATTRIB_VEC4_COLOR], v.r, v.g, v.b, v.a);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->id);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}