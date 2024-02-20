#include "Patch.h"

Patch::Patch(
	glm::vec2 input_corner_data,
	glm::vec3 input_offset,
	int input_size,
	int input_subdivision,
	float input_amplitude,
	Noise* input_noise,
	std::vector<Texture*> input_textures
) {
	corner_data = input_corner_data;
	offset = input_offset;
	size = input_size;
	subdivision = input_subdivision;
	amplitude = input_amplitude;
	noise = input_noise;
	textures = input_textures;
	useErosion = false;
	GenerateVertices();
}

float Patch::GetHeight(int vertexID, float scalar) {
	int i = floor(vertexID / subdivision);
	int j = vertexID % subdivision;

	float x = ((float)i * scalar) - ((float)size / 2);
	float z = ((float)j * scalar) - ((float)size / 2);

	float y = noise->get(offset[0] + x, offset[2] + z, useErosion) * amplitude;

	int iRank = i % 3;
	int jRank = j % 3;

	// Handle z axis patch seaming 
	if ((i == 0 and corner_data[0] == -1.0f) or (i == subdivision-1 and corner_data[0] == 1.0f) and jRank != 0) {
		// The process is to basically sample at 3 times the current patch size and interpolate between the two points
		//                    o--------O y2
		//           ---------^ use this point where y = 1/3 y1 + 2/3 y2
		//  O--------
		//  y1
		float z1 = ((float)(j - jRank) * scalar) - ((float)size / 2);
		float z2 = ((float)(j + 3 - jRank) * scalar) - ((float)size / 2);
		float y1 = noise->get(offset[0] + x, offset[2] + z1, useErosion) * amplitude;
		float y2 = noise->get(offset[0] + x, offset[2] + z2, useErosion) * amplitude;
		y = (1.0f - ((float)jRank / 3.0f)) * y1 + ((float)jRank / 3.0f) * y2;
	}
	// Handle x axis patch seaming 
	else if ((j == 0 and corner_data[1] == -1.0f) or (j == subdivision-1 and corner_data[1] == 1.0f) and iRank != 0) {
		float x1 = ((float)(i - iRank) * scalar) - ((float)size / 2);
		float x2 = ((float)(i + 3 - iRank) * scalar) - ((float)size / 2);
		float y1 = noise->get(offset[0] + x1, offset[2] + z, useErosion) * amplitude;
		float y2 = noise->get(offset[0] + x2, offset[2] + z, useErosion) * amplitude;
		y = (1.0f - ((float)iRank / 3.0f)) * y1 + ((float)iRank / 3.0f) * y2;
	}

	return y;
}

void Patch::GenerateVertices() {
	// Clear the vertices and indices vectors
	vertices.clear();
	indices.clear();

	int num_vertices = subdivision * subdivision;
	float scalar = (float)size / ((float)subdivision - 1);
	int max = subdivision - 1;

	//std::cout << num_vertices - 1 << std::endl;

	// Used to keep track of the current vertex so that indice instantiation is correct
	for (int i = 0; i < num_vertices; i++) {
		float y = GetHeight(i, scalar);
		vertices.push_back(StupidVertex
			{
				y
			}
		);
		if (floor(i / subdivision) < max and i % subdivision < max) {
			indices.push_back(i);
			indices.push_back(i + subdivision + 1);
			indices.push_back(i + 1);

			indices.push_back(i);
			indices.push_back(i + subdivision);
			indices.push_back(i + subdivision + 1);
		}
	}

	VAO.Bind();
	// Generates GrassVertex Buffer Object and links it to vertices
	VBO VBO(vertices);
	// Generates Element Buffer Object and links it to indices
	EBO EBO(indices);

	// arg[1] = Specifies the index of the generic vertex attribute to be modified.
	// arg[2] = Specifies the number of components per generic vertex attribute.
	// arg[3] GL_FLOAT = type of data (i.e. float or int)
	// arg[4] sizeof(PatchVertex) = Specifies the byte offset between consecutive generic vertex attributes. If stride is 0, the generic vertex 
	// attributes are understood to be tightly packed in the array. The initial value is 0.
	// arg[5] offset = Specifies a offset of the first component of the first generic vertex attribute in the array in the data store of the buffer 
	// currently bound to the GL_ARRAY_BUFFER target. The initial value is 0.
	VAO.LinkAttrib(VBO, 0, 1, GL_FLOAT, sizeof(StupidVertex), (void*)0);

	// Unbind all to prevent accidentally modifying them
	VAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();
}

void Patch::Draw
(
	Shader& shader,
	Camera& camera
)
{
	// Bind shader to be able to access uniforms
	shader.Activate();
	// Bind the VAO so OpenGL knows to use it
	VAO.Bind();

	// Binds textures so that they appear in the rendering
	// Keep track of how many of each type of textures we have
	unsigned int numDiffuse = 0;
	unsigned int numSpecular = 0;
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		std::string num;
		std::string type = textures[i]->type;
		if (type == "diffuse")
		{
			num = std::to_string(numDiffuse++);
		}
		else if (type == "specular")
		{
			num = std::to_string(numSpecular++);
		}
		textures[i]->texUnit(shader, (type + num).c_str(), i);
		textures[i]->Bind();
	}

	// Push the matrices to the vertex shader
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "translation"), 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(1.0f), offset)));

	glUniform1i(glGetUniformLocation(shader.ID, "size"), size);

	float scalar = (float)size / (float)(subdivision - 1);
	glUniform1f(glGetUniformLocation(shader.ID, "scalar"), scalar);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}