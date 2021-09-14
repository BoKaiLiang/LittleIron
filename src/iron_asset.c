#include "iron_asset.h"

#include <stdio.h>

#include "glad/gl.h"

#if 0
// vertex shader
	unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertsrc, NULL);
	glCompileShader(vertex_shader);
	// check for shader compile errors
	int success;
	char info_log[512];
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderinfo_log(vertex_shader, 512, NULL, info_log);
		LOG_ERROR("ERROR::SHADER::VERTEX::COMPILATION_FAILED: %s", info_log);
        return RES_FAILED;
	}
	// fragment shader
	unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragsrc, NULL);
	glCompileShader(fragment_shader);
	// check for shader compile errors
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderinfo_log(fragment_shader, 512, NULL, info_log);
		LOG_ERROR("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED: %s", info_log);
        return RES_FAILED;
	}
	// link shaders
	unsigned int shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	// check for linking errors
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetPrograminfo_log(shader_program, 512, NULL, info_log);
		LOG_ERROR("ERROR::SHADER::PROGRAM::LINKING_FAILED: %s", info_log);
        return RES_FAILED;
	}
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

    // set shader program
	// if (shader == NULL)
	// 		shader = malloc(sizeof(Shader));
	shader->ID = shader_program;

	shader->locations = calloc(MAX_SHADER_ATTR, sizeof(unsigned int));

	int position_location = glGetAttribLocation(shader->ID, "_Pos");
	if (-1 == position_location) {
		LOG_ERROR("ERROR::SHADER::Can't find position uniform location");
        return RES_FAILED;
	}
	shader->locations[SHADER_ATTR_LOC_VEC3_POSITIONS] = position_location;

	int texcoord_location = glGetAttribLocation(shader->ID, "_Texcoord");
	if (-1 == texcoord_location) {
		LOG_ERROR("ERROR::SHADER::Can't find texture coordinate uniform location");
        return RES_FAILED;
	}
	shader->locations[SHADER_ATTR_LOC_VEC2_TEXCOORDS] = texcoord_location;

	int color_location = glGetAttribLocation(shader->ID, "_Color");
	if (-1 == color_location) {
		LOG_ERROR("ERROR::SHADER::Can't find color uniform location");
        return RES_FAILED;
	}
	shader->locations[SHADER_ATTR_LOC_VEC4_COLORS] = color_location;

	int mvp_location = glGetUniformLocation(shader->ID, "_MVP");
	if (-1 == mvp_location) {
		LOG_ERROR("ERROR::SHADER::Can't find mvp uniform location");
        return RES_FAILED;
	}
	shader->locations[SHADER_ATTR_LOC_MAT4_MVP] = mvp_location;

	int texture_location = glGetUniformLocation(shader->ID, "_Texture2D");
	if (-1 == texture_location) {
		LOG_ERROR("ERROR::SHADER::Can't find texture uniform location");
        return RES_FAILED;
	}
	shader->locations[SHADER_ATTR_LOC_SAMPLER2D_TEX] = texture_location;

	return RES_OK;
#endif

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
		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED:\n\t%s\n", info_log);
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
		printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED:\n\t%s\n", info_log);
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
		printf("ERROR::SHADER::PROGRAM::LINKING_FAILED:\n\t%s", info_log);
        return RES_ERROR_CREATE_SHADER_PROGRAM;
	}
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	// Get shader's attribute, uniform location
	int pos_location = glGetAttribLocation(shader->id, "_Pos");
	if (pos_location <= 0) {
		return RES_ERROR_GET_SHADER_ATTRIBUTE;
	}
	shader->attribs_locations[SHADER_ATTRIB_VEC2_POS] = pos_location;

	int color_location = glGetUniformLocation(shader->id, "_Color");
	if (color_location <= 0) {
		return RES_ERROR_GET_SHADER_UNIFORM;
	}
	shader->attribs_locations[SHADER_ATTRIB_VEC2_POS] = color_location;


    return RES_SUCCESS;
}

// [iron_asset] release shader
void ReleaseShader(Shader shader) {
    glDeleteProgram(shader.id);
}


// [iron_asset] load image file and store to opengl texture
ResT LoadTextureFile(Texture* texture, const char* file_name) {
	// TODO...
}

// [iron_asset] release texture
void ReleaseTexture(Texture* texture) {
	// TODO...
}