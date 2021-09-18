#include "iron_asset.h"

#include <stdio.h>

#include "glad/gl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

// [iron_asset] load shader text content for creating shader program
ResT LoadShaderCode(Shader* shader, const char* vertex_code, const char* fragment_code) {

    // vertex shader
	unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_code, NULL);
	glCompileShader(vertex_shader);
	// check for shader compile errors
	int success;
	char info_log[512];
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
		LogError("Failed to compile \"vertex\" shader, see error log:\n%s", info_log);
        return RES_ERROR_LOAD_VERTEX_SHADER;
	}
	// fragment shader
	unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_code, NULL);
	glCompileShader(fragment_shader);
	// check for shader compile errors
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
		LogError("Failed to compile \"fragment\" shader, see error log:\n%s", info_log);
        return RES_ERROR_LOAD_FRAGMENT_SHADER;
	}
	// link shaders
	shader->id = glCreateProgram();
	glAttachShader(shader->id, vertex_shader);
	glAttachShader(shader->id, fragment_shader);
	glLinkProgram(shader->id);
	// check for linking errors
	glGetProgramiv(shader->id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shader->id, 512, NULL, info_log);
		LogError("Failed to linked shader program, see error log:\n%s", info_log);
        return RES_ERROR_CREATE_SHADER_PROGRAM;
	}
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	// Get shader's attribute, uniform location
	int pos_location = glGetAttribLocation(shader->id, "_Pos");
	shader->attribs_locations[SHADER_ATTRIB_VEC2_POS] = pos_location;

	int texcoord_location = glGetAttribLocation(shader->id, "_Texcoords");
	shader->attribs_locations[SHADER_ATTRIB_VEC2_TEXCOORD] = texcoord_location;

	int color_location = glGetUniformLocation(shader->id, "_Color");
	shader->attribs_locations[SHADER_ATTRIB_VEC4_COLOR] = color_location;

	int texture_location = glGetUniformLocation(shader->id, "_Texture2D");
	shader->attribs_locations[SHADER_ATTRIB_SAMPLER2D_TEXTURE] = texcoord_location;

    return RES_SUCCESS;
}

// [iron_asset] release shader
void ReleaseShader(Shader* shader) {
	if (shader != NULL) {
    	glDeleteProgram(shader->id);
	}
}


// [iron_asset] load image file and store to opengl texture
ResT LoadTextureFile(Texture* texture, const char* file_name) {
	int channels;
	unsigned char* pixels = stbi_load(file_name, &texture->w, &texture->h, &channels, 0);
	if (pixels == NULL) {
		stbi_image_free(pixels);
		LogError("Failed to load image, it file name and path: %s", file_name);
		return RES_ERROR_LOAD_IMAGE_FILE;
	}

	int src_img_fmt;
	if (channels == 2) {
		texture->gl_fmt = TEX_FMT_GRAYSCALE;
		src_img_fmt = GL_RG8;
	} else if (channels == 3) {
		texture->gl_fmt = TEX_FMT_RGB;
		src_img_fmt = GL_RGB8;
	} else if (channels == 4) {
		texture->gl_fmt = TEX_FMT_RGBA;
		src_img_fmt = GL_RGBA8;
	} else {
		texture->gl_fmt = TEX_FMT_WHITE_BLACK;
		src_img_fmt = GL_R8;
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &texture->id);
	glBindTexture(GL_TEXTURE_2D, texture->id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, src_img_fmt, texture->w, texture->h, 0, texture->gl_fmt, GL_UNSIGNED_BYTE, pixels);

	glBindTexture(GL_TEXTURE_2D, 0);

	// free the pixel data
	stbi_image_free(pixels);

	return RES_SUCCESS;
}

// [iron_asset] release texture
void ReleaseTexture(Texture* texture) {
	if (texture != NULL) {
		glDeleteTextures(1, &texture->id);
	}
}