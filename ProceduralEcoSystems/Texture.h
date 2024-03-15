#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include<glad/glad.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include"shaderClass.h"
#include"Noise.h"
#include"FastPoissonDiskSampling.h"


class Texture
{
public:
	GLuint ID;
	const char* type;
	GLuint unit;

	Texture();

	Texture(const char* image, const char* texType, GLuint slot);

	Texture(const char* texType, GLuint slot);

	Texture(const char* texType, GLuint slot, std::vector<float> layer_radii);

	Texture(Noise* noise, int type, float steepness_scalar, const char* texType, GLuint slot);

	void texUnit(Shader& shader, const char* uniform, GLuint unit);
	void Bind();
	void Unbind();
	void Delete();
};
#endif