#version 330 core

// Positions/Coordinates
layout (location = 0) in float height;
layout (location = 1) in vec3 normal;

// Outputs the current position for the Fragment Shader
out vec3 crntPos;
// Outputs the normal for the Fragment Shader
out vec3 Normal;
// Outputs the color for the Fragment Shader
out vec3 color;
// Outputs the texture coordinates to the Fragment Shader
out vec2 texCoord;



// Imports the camera matrix
uniform mat4 camMatrix;
// Imports the transformation matrices
uniform mat4 translation;
uniform float scalar;
uniform int subdivision;
uniform int size;

float modI(float a,float b) { //  Returns accurate MOD when arguments are approximate integers.
    float m=a-floor((a+0.5)/b)*b;
    return floor(m+0.5);
}

void main()
{
	// calculates current position

	float xPos = floor(gl_VertexID / subdivision);
	float zPos = modI(float(gl_VertexID), float(subdivision));
	
	float xScaled = xPos * scalar - float(size) * 0.5;
	float zScaled = zPos * scalar - float(size) * 0.5;
	
    vec4 position = vec4(xScaled, height, zScaled, 1.0);
	crntPos = vec3(translation * position);

	// Assigns the normal from the Vertex Data to "Normal"
	Normal = normal;
	// Assigns the colors from the Vertex Data to "color"
	color = vec3(0.0, 1.0, 0.0);
	// Assigns the texture coordinates from the Vertex Data to "texCoord"
    texCoord = vec2(xScaled, zScaled);
	
	// Outputs the positions/coordinates of all vertices
	gl_Position = camMatrix * vec4(crntPos, 1.0);
}