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
uniform int maxLeaves;
uniform int vertices_per_leaf;
uniform float pitchVariance;
uniform float bendVariance;
uniform float lengthVariance;
uniform float scaleVariance;
uniform float scale;

float random (vec2 st) {
    return 0.5-fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

float modI(float a,float b) { //  Returns accurate MOD when arguments are approximate integers.
    float m=a-floor((a+0.5)/b)*b;
    return floor(m+0.5);
}

void main()
{
	// calculates current position
	crntPos = vec3(0.0);
	int curr_vertex = int(modI(gl_VertexID, vertices_per_leaf));
	int curr_leaf = int(floor(gl_VertexID / vertices_per_leaf));
	
    float rnd = random( vec2(curr_leaf * 2.414213562, curr_leaf * 2.04487392765219) );

	// Get the width and distance of the leaf
	float width = modI((curr_vertex), 2)-0.5;
	float distance = (leafLength + (lengthVariance*rnd)) * floor((curr_vertex) / 2.0) / vertices_per_leaf;

	// Animate the leaf
	float time_l = time - (distance * 0.25f) - (curr_vertex/vertices_per_leaf);
	float wind = sin(time_l) - sin(time_l/2) + sin(time_l/4) - sin(time_l/8);

	// Get the pitch and yaw of the leaf
	float newYaw = yaw + (float(6.28/maxLeaves) * rnd * curr_leaf);
	float bendPitch = (pitch + (pitchVariance*rnd)) - distance * (bendStrength + (bendVariance*rnd)) + wind * 0.04;

	crntPos.x = cos(newYaw) * -width + cos(bendPitch) * distance * sin(newYaw);
	crntPos.y = sin(bendPitch) * distance;
	crntPos.z = sin(newYaw) * width + cos(bendPitch) * distance * cos(newYaw);

	// Assigns the normal from the Vertex Data to "Normal"
	Normal = vec3(0.0, 1.0, 0.0);
	// Assigns the colors from the Vertex Data to "color"
	color = vec3(0.0, 0.0, 0.0);
	// Assigns the texture coordinates from the Vertex Data to "texCoord"
	texCoord = mat2(0.0, -1.0, 1.0, 0.0) * vec2( floor((curr_vertex) / 2.0) / (segments-1), width + 0.5 );
	
	// Outputs the positions/coordinates of all vertices
	gl_Position = camMatrix * vec4(crntPos*vec3(scale+(rnd*scaleVariance)) + aPos, 1.0);
}