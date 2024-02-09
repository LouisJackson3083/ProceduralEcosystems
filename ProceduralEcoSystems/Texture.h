#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include<glad/glad.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include"shaderClass.h"
#include"Noise.h"


class Texture
{
public:
	GLuint ID;
	const char* type;
	GLuint unit;

	Texture();

	Texture(const char* image, const char* texType, GLuint slot);

	Texture(Noise* noise, int type, float steepness_scalar, bool useErode, const char* texType, GLuint slot);

	// Assigns a texture unit to a texture
	void texUnit(Shader& shader, const char* uniform, GLuint unit);
	// Binds a texture
	void Bind();
	// Unbinds a texture
	void Unbind();
	// Deletes a texture
	void Delete();
};
#endif