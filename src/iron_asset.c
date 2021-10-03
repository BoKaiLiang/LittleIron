#include "iron_asset.h"

#include <stdio.h>

#include "glad/gl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_truetype.h"

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

	int color_location = glGetAttribLocation(shader->id, "_Color");
	shader->attribs_locations[SHADER_ATTRIB_VEC4_COLOR] = color_location;

	int texture_location = glGetUniformLocation(shader->id, "_Texture2D");
	shader->attribs_locations[SHADER_ATTRIB_SAMPLER2D_TEXTURE] = texture_location;

	int mvp_location = glGetUniformLocation(shader->id, "_MVP");
	shader->attribs_locations[SHADER_ATTRIB_MAT4_MVP] = mvp_location;

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

// [iron_asset] load texture from image
ResT LoadTextureImg(Texture* texture, Image* img) {
	glBindTexture(GL_TEXTURE_2D, 0);

	texture->w = img->w;
	texture->h = img->h;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texture->id);
	glBindTexture(GL_TEXTURE_2D, texture->id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (img->src_fmt == IMG_FMT_WHITE_BLACK) texture->gl_fmt = TEX_FMT_WHITE_BLACK;
	else if (img->src_fmt == IMG_FMT_GRAYSACLE) texture->gl_fmt = TEX_FMT_GRAYSCALE;
	else if (img->src_fmt == IMG_FMT_RGB) texture->gl_fmt = TEX_FMT_RGB;
	else if (img->src_fmt == IMG_FMT_RGBA) texture->gl_fmt = TEX_FMT_RGB;

	if (img->src_fmt == IMG_FMT_WHITE_BLACK) {
		int swizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_RED };
		glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
	}
	else if (img->src_fmt == IMG_FMT_GRAYSACLE) {
		int swizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_GREEN };
		glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, img->src_fmt, texture->w, texture->h, 0, texture->gl_fmt, GL_UNSIGNED_BYTE, img->data);

	glBindTexture(GL_TEXTURE_2D, 0);

	return RES_SUCCESS;
}

// [iron_asset] release texture
void ReleaseTexture(Texture* texture) {
	if (texture != NULL) {
		glDeleteTextures(1, &texture->id);
	}
}

ResT LoadFontTTF(Font* font, int default_sz, const char* file_name) {
	font->base_height = default_sz;
	font->texture = (Texture){ 0 };
	memset(font->chars, 0, sizeof(CharInfo) * ASCII_CHAR_COUNT);

	FILE* font_file = fopen(file_name, "rb");
	if (font_file == NULL) {
		LogError("Failed to load font: %s", file_name);
		return RES_ERROR_LOAD_TTF_FILE;
	}

	fseek(font_file, 0, SEEK_END);
	long size = ftell(font_file);    // Get file size
	fseek(font_file, 0, SEEK_SET);   // Reset file pointer

	unsigned char* font_buffer = (unsigned char*)malloc(size);

	fread(font_buffer, size, 1, font_file);
	fclose(font_file);

	// Init font for data reading
	stbtt_fontinfo font_info;
	if (!stbtt_InitFont(&font_info, font_buffer, 0)) {
		LogError("Failed to init font: %s", file_name);
		return RES_ERROR_CREATE_STB_TTF_OBJ;
	}

	float scale = stbtt_ScaleForPixelHeight(&font_info, (float)font->base_height);

	int ascent = 0;
    int descent = 0;
    int line_gap = 0;
    stbtt_GetFontVMetrics(&font_info, &ascent, &descent, &line_gap);

	for (int i = 0; i < ASCII_CHAR_COUNT; i++) {

		int unicode = i + 32;
		font->chars[i].unicode = unicode;

		int chw = 0, chh = 0;

		font->chars[i].data 
			= stbtt_GetCodepointBitmap(&font_info, scale, scale, unicode, &chw, &chh, &font->chars[i].xoffset, &font->chars[i].yoffset);

		font->chars[i].rec.w = (float)chw;
		font->chars[i].rec.h = (float)chh;
		font->chars[i].yoffset += (int)((float)ascent * scale);

		stbtt_GetCodepointHMetrics(&font_info, unicode, &font->chars[i].xadvance, NULL);
		font->chars[i].xadvance *= scale;

	}

	Image font_atlas = (Image){ 0 };

	float rq_area = 0;
	for (int i = 0; i < ASCII_CHAR_COUNT; i++) {
		rq_area += (font->chars[i].rec.w) * (font->chars[i].rec.h);
	}
	float measured_sz = sqrtf(rq_area) * 1.25f;
	int img_sz = (int)powf(2, ceilf(logf((float)measured_sz) / logf(2)));  // Calculate next POT

	font_atlas.h = img_sz;
	font_atlas.w = img_sz;
	unsigned char* font_data = calloc(img_sz * img_sz, sizeof(unsigned char));

    int padding = 2;

	stbrp_context* context = (stbrp_context*)malloc(sizeof(stbrp_context));
	stbrp_node* nodes = (stbrp_node*)malloc(sizeof(ASCII_CHAR_COUNT * sizeof(stbrp_node)));

	stbrp_init_target(context, font_atlas.w, font_atlas.h, nodes, ASCII_CHAR_COUNT);
	stbrp_rect* rects = (stbrp_rect*)malloc(ASCII_CHAR_COUNT * sizeof(stbrp_rect));

	for (int i = 0; i < ASCII_CHAR_COUNT; i++) {
		rects[i].id = i;
		rects[i].w = font->chars[i].rec.w + 2 * padding;
		rects[i].h = font->chars[i].rec.h + 2 * padding;
	}

	stbrp_pack_rects(context, rects, ASCII_CHAR_COUNT);

	for (int i = 0; i < ASCII_CHAR_COUNT; i++) {
		font->chars[i].rec.x = rects[i].x + (float)padding;
		font->chars[i].rec.y = rects[i].y + (float)padding;
		font->chars[i].rec.w = (float)font->chars[i].rec.w;
		font->chars[i].rec.h = (float)font->chars[i].rec.h;

		if (rects[i].was_packed)
		{
			for (int y = 0; y < (int)font->chars[i].rec.h; y++) {
				for (int x = 0; x < (int)font->chars[i].rec.w; x++) {
					font_data[(rects[i].y + padding + y) * font_atlas.w + (rects[i].x + padding + x)] = font->chars[i].data[y * (int)font->chars[i].rec.w + x];
				}	
			}
		} else {
			LogWarning("FONT: Failed to package character (%i) -> ascii: %c", i, (char)i);
		}
	}

	free(rects);
	free(nodes);
	free(context);

	unsigned char* data_gray_alpha = (unsigned char*)malloc( sizeof(unsigned char) * img_sz * img_sz * 2); // Two channels

	for (int i = 0, k = 0; i < (font_atlas.w * font_atlas.h); i++, k += 2)
	{
		data_gray_alpha[k] = 255;
		data_gray_alpha[k + 1] = (font_data)[i];
	}

	font_atlas.data = data_gray_alpha;
	font_atlas.src_fmt = IMG_FMT_GRAYSACLE;

	ResT res = LoadTextureImg(&font->texture, &font_atlas);
	if (res != RES_SUCCESS) {
		LogError("Failed to create font to texture");
		return RES_ERROR_CREATE_FONT_TEXTURE;
	}

	free(font_atlas.data);
	free(font_buffer);

	return RES_SUCCESS;
}

void ReleaseFont(Font* font) {
	if (font != NULL) {
		glDeleteTextures(1, &font->texture.id);
		for (int i = 0; i < ASCII_CHAR_COUNT; i++) {
			free(font->chars[i].data);
		}
	}
}