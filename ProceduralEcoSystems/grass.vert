#version 330 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;

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
uniform float pitch;
uniform float yaw;
uniform float bendStrength;
uniform float time;
uniform int segments;
uniform int leafLength;
uniform int leaves;

float rand(float seed){
    return fract(sin(dot(vec2(seed,seed), vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
	// calculates current position
	crntPos = vec3(0.0);
	int vertices_per_leaf = segments*2;
	int curr_vertex = int(mod(gl_VertexID, vertices_per_leaf));
	if (curr_vertex > vertices_per_leaf-1) {curr_vertex = 0;}

	// Get the width of leaf
	float width = mod((curr_vertex), 2)-0.5;
	float distance = leafLength * floor((curr_vertex) / 2.0) / vertices_per_leaf;

	// Animate the leaf
	float time_l = time - (distance * 0.25f) - (curr_vertex/vertices_per_leaf);
	float wind = sin(time_l) - sin(time_l/2) + sin(time_l/4) - sin(time_l/8);

	float newYaw = yaw * int(floor(gl_VertexID / vertices_per_leaf));
	float bendPitch = pitch - distance * bendStrength + wind * 0.04;

	crntPos.x = cos(newYaw) * -width + cos(bendPitch) * distance * sin(newYaw);
	crntPos.y = sin(bendPitch) * distance;
	crntPos.z = sin(newYaw) * width + cos(bendPitch) * distance * cos(newYaw);


	// Assigns the normal from the Vertex Data to "Normal"
	Normal = vec3(0.0, 1.0, 0.0);
	// Assigns the colors from the Vertex Data to "color"
	color = vec3(0.0, 0.0, 0.0);
	// Assigns the texture coordinates from the Vertex Data to "texCoord"
	texCoord = mat2(0.0, -1.0, 1.0, 0.0) * vec2( floor((curr_vertex) / 2.0) / segments, width + 0.5 );
	
	// Outputs the positions/coordinates of all vertices
	gl_Position = camMatrix * vec4(crntPos, 1.0);
}