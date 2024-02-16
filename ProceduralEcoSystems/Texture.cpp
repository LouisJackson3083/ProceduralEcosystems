#include"Texture.h"

Texture::Texture(const char* image, const char* texType, GLuint slot)
{
	// Assigns the type of the texture ot the texture object
	type = texType;

	// Stores the width, height, and the number of color channels of the image
	int widthImg, heightImg, numColCh;
	// Flips the image so it appears right side up
	stbi_set_flip_vertically_on_load(true);
	// Reads the image from a file and stores it in bytes
	unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);

	// Generates an OpenGL texture object
	glGenTextures(1, &ID);
	// Assigns the texture to a Texture Unit
	glActiveTexture(GL_TEXTURE0 + slot);
	unit = slot;
	glBindTexture(GL_TEXTURE_2D, ID);

	// Configures the type of algorithm that is used to make the image smaller or bigger
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Configures the way the texture repeats (if it does at all)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Extra lines in case you choose to use GL_CLAMP_TO_BORDER
	// float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);

	// Check what type of color channels the texture has and load it accordingly
	if (numColCh == 4)
		glTexImage2D
		(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			widthImg,
			heightImg,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			bytes
		);
	else if (numColCh == 3)
		glTexImage2D
		(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			widthImg,
			heightImg,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			bytes
		);
	else if (numColCh == 1)
		glTexImage2D
		(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			widthImg,
			heightImg,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			bytes
		);
	else
		throw std::invalid_argument("Automatic Texture type recognition failed");

	// Generates MipMaps
	glGenerateMipmap(GL_TEXTURE_2D);

	// Deletes the image data as it is already in the OpenGL Texture object
	stbi_image_free(bytes);

	// Unbinds the OpenGL Texture object so that it can't accidentally be modified
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(Noise* noise, int type, float steepness_scalar, bool useErode, const char* texType, GLuint slot)
{
	const int texSize = 256;
	GLfloat noiseImage[texSize][texSize];

	if (type == 0) { // A normal noise image
		for (int i = 0; i < texSize; i++) {
			for (int j = 0; j < texSize; j++) {
				noiseImage[i][j] = (GLfloat)noise->get(i, j, false);
			}
		}
	}
	else if (type == 1) { // A gradient noise image
		GLfloat highest_Value = 0.0f;
		for (int i = 0; i < texSize; i++) {
			for (int j = 0; j < texSize; j++) {
				float slopeX = noise->get(i + steepness_scalar, j, false) - noise->get(i - steepness_scalar, j, false);
				float slopeZ = noise->get(i, j + steepness_scalar, false) - noise->get(i, j - steepness_scalar, false);
				float steepness = glm::dot(glm::normalize(glm::vec3(slopeX, 1.0f, slopeZ)), glm::vec3(0.0f, 1.0f, 0.0f));
				noiseImage[i][j] = (GLfloat)(glm::acos(steepness) * 2.0f * steepness_scalar);
				if (noiseImage[i][j] > highest_Value) {
					highest_Value = noiseImage[i][j];
				}
			}
		}
	}
	else if (type == 2) { // An erosion image
		for (int i = 0; i < texSize; i++) {
			for (int j = 0; j < texSize; j++) {
				if (useErode) {
					noiseImage[i][j] = (GLfloat)(noise->erosionMap[i][j] * 255.0f);
				}
				else {
					noiseImage[i][j] = (GLfloat)(noise->get(i, j, false) / 2.0f);
				}
			}
		}
	}
	else if (type == 3) { // An erosion + noise image
		for (int i = 0; i < texSize; i++) {
			for (int j = 0; j < texSize; j++) {
				if (useErode) { 
					noiseImage[i][j] = (GLfloat)((noise->get(i, j, false) / 2.0f) + (noise->erosionMap[i][j] * 255.0f)); 
				}
				else {
					noiseImage[i][j] = (GLfloat)(noise->get(i, j, false) / 2.0f);
				}
			}
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Generates an OpenGL texture object
	glGenTextures(1, &ID);

	glBindTexture(GL_TEXTURE_2D, ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, texSize, texSize, 0, GL_RED, GL_FLOAT, noiseImage);
	// Unbinds the OpenGL Texture object so that it can't accidentally be modified
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(const char* texType, GLuint slot) {
	const int texSize = 256;
	GLubyte plantImage[texSize][texSize][4]; // Modify to hold RGBA values
	int c;
    for (int i = 0; i < texSize; i++) {
        for (int j = 0; j < texSize; j++) {
			c = (((i & 0x8) == 0) ^ ((j & 0x8)) == 0) * 255;
			plantImage[i][j][0] = (GLubyte)0;
			plantImage[i][j][1] = (GLubyte)255;
			plantImage[i][j][2] = (GLubyte)0;
			plantImage[i][j][3] = (GLubyte)255;
        }
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSize, texSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, plantImage);

    // Unbinds the OpenGL Texture object so that it can't accidentally be modified
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{
	// Gets the location of the uniform
	GLuint texUni = glGetUniformLocation(shader.ID, uniform);
	// Shader needs to be activated before changing the value of a uniform
	shader.Activate();
	// Sets the value of the uniform
	glUniform1i(texUni, unit);
}

void Texture::Bind()
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}