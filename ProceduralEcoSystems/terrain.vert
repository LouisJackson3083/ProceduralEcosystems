#version 330 core

// Positions/Coordinates
layout (location = 0) in float height;

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
uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

uniform int subdivision;

void main()
{
	// calculates current position
	vec3 position = vec3(0.0);

	position.x = floor(gl_VertexID / subdivision);
	position.z = mod(gl_VertexID, subdivision);

	crntPos = vec3(model * translation * rotation * scale * vec4(position, 1.0f));
	// Assigns the normal from the Vertex Data to "Normal"
	Normal = vec3(0.0, 1.0, 0.0);
	// Assigns the colors from the Vertex Data to "color"
	color = vec3(0.0, 0.0, 0.0);
	// Assigns the texture coordinates from the Vertex Data to "texCoord"
	texCoord = mat2(0.0, -1.0, 1.0, 0.0) * vec2(position.x / (subdivision-1), position.z / (subdivision-1));
	
	// Outputs the positions/coordinates of all vertices
	gl_Position = camMatrix * vec4(crntPos, 1.0);
}