#ifndef GRASS_CLASS_H
#define GRASS_CLASS_H

#include<vector>
#include<glad/glad.h>
#include<glm/glm.hpp>
#include"Texture.h"
#include"Camera.h"
#include"EBO.h"
#include"VBO.h"
#include"VAO.h"
#include <random>
#include"Noise.h"

class Grass {
public:

	std::vector<Texture> textures;
	std::vector<Texture> textures2;
	std::vector<PlantVertex> vertices;
	std::vector<GLuint> indices;
	std::vector<PlantVertex> lowPolyVertices;
	std::vector<GLuint> lowPolyIndices;
	VAO vao;
	VAO lowPolyvao;

	std::vector<glm::vec2> positions;
	std::vector<glm::vec2> positionsLowPoly;

	float length;
	float lengthVariance;
	float pitchVariance;
	float scale;
	float scaleVariance;

	// Ecosystem parameters
	int ecosystemDominance;
	float ecosystemOxygenUpperLimit;
	float ecosystemOxygenLowerLimit;
	float ecosystemRootingStrength;
	float ecosystemMoistureRequirement;
	float ecosystemInteractionLevel;

	Noise* noise;

	Grass(Noise* input_noise);

	void GenerateVertices();


	void Draw(
		Shader& shader,
		Shader& shader2,
		Camera& camera,
		glm::mat4 matrix = glm::mat4(1.0f),
		glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f)
	);
};

#endif