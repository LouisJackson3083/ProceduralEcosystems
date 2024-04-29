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
uniform float height;
uniform float radius;
uniform int vertices_per_branch;
uniform int branchSegments;
uniform float pitch;
uniform int maxBranches;
uniform float time;
uniform float bendStrength;
uniform int branchLength;
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

	int curr_vertex = int(modI(gl_VertexID, vertices_per_branch));
	int curr_branch = int(floor(gl_VertexID / vertices_per_branch));
	
    float rnd = random( vec2(curr_branch * aPos.x * 2.414213562, curr_branch * aPos.z * 5.04487392765219) );

	// Get the width and distance of the branch
	float width = modI((curr_vertex), 2)-0.5;
	float distance = (branchLength + (lengthVariance*rnd)) * floor((curr_vertex) / 2.0) / vertices_per_branch;
	
	// Animate the branch
	float time_l = time + aPos.x + aPos.z - (distance * 0.25f) - (curr_vertex/vertices_per_branch);
	float wind = sin(time_l) - sin(time_l/2) + sin(time_l/4) - sin(time_l/8);
	
	// Get the pitch and yaw of the leaf
	float yaw = (float(6.28/maxBranches) * curr_branch) + rnd;
	float bendPitch = (pitch + (pitchVariance*rnd)) - distance * (bendStrength + (bendVariance*rnd)) + wind * 0.04;
	
	crntPos.x = cos(yaw) * -width + cos(bendPitch) * distance * sin(yaw);
	crntPos.y = sin(bendPitch) * distance;
	crntPos.z = sin(yaw) * width + cos(bendPitch) * distance * cos(yaw);

	// Assigns the normal from the Vertex Data to "Normal"
	Normal = vec3(0.0, 1.0, 0.0);
	// Assigns the colors from the Vertex Data to "color"
	color = vec3(0.0, 0.0, 0.0);
	// Assigns the texture coordinates from the Vertex Data to "texCoord"
	texCoord = mat2(0.0, -1.0, 1.0, 0.0) * vec2( floor((curr_vertex) / 2.0) / (branchSegments-1), width + 0.5 );
	
	// Outputs the positions/coordinates of all vertices
	gl_Position = camMatrix * vec4(crntPos*vec3(scale+(rnd*scaleVariance)) + aPos, 1.0);
}