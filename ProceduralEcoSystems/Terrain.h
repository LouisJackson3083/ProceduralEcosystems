#ifndef TERRAIN_CLASS_H
#define TERRAIN_CLASS_H

#include"Mesh.h"

class Terrain
{
public:
	Terrain();

	void Draw
	(
		Shader& shader,
		Camera& camera
	);

	void GenQuad(int displacement);

private:
	// All the meshes and transformations
	std::vector<Mesh> meshes;
	std::vector<glm::mat4> matricesMeshes;
	std::vector<glm::mat4> instanceMatrix;


	// Prevents textures from being loaded twice
	std::vector<std::string> loadedTexName;
	std::vector<Texture> loadedTex;

	// Holds number of instances (if 1 the mesh will be rendered normally)
	unsigned int instancing;
};



#endif