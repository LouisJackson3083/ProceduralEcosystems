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

	// Initializes the skybox
	Skybox();

	// Draws the skybox
	void Draw
	(
		Shader& shader,
		Camera& camera,
		float width,
		float height
	);
};

#endif