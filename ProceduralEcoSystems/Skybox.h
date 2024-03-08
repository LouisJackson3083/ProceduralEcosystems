#ifndef SKYBOX_CLASS_H
#define SKYBOX_CLASS_H

#include"Camera.h"
#include"Texture.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"

class Skybox
{
public:
	unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
	unsigned int cubemapTexture;
	float width;
	float height;

	// Initializes the skybox
	Skybox(float input_width, float input_height);

	// Draws the skybox
	void Draw
	(
		Shader& shader,
		Camera& camera
	);
};

#endif