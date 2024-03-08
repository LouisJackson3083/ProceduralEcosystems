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
uniform float time;
uniform float bladeLength;
uniform float pitchVariance;
uniform float lengthVariance;

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
	int curr_vertex = int(modI(gl_VertexID, 4));
	int curr_leaf = int(floor(gl_VertexID / 4));
	
	float rnd = random( vec2(curr_vertex+curr_leaf * 2.414213562, curr_vertex+curr_leaf * 2.04487392765219) );
	
	float width = modI((curr_vertex), 2)/4;
	float dist = (bladeLength + (lengthVariance*rnd)) * floor((curr_vertex) / 2.0);

	// Animate the leaf
	float time_l = time - (dist * 0.25f);
	float wind = sin(time_l) - sin(time_l/2) + sin(time_l/4) - sin(time_l/8);

	// Get the pitch and yaw of the leaf
	float newYaw = float((6.28/rnd) * curr_leaf);
	float newPitch = ((modI(gl_VertexID, 2))-0.5) * pitchVariance*rnd - dist + wind * 0.04;
	
	crntPos.x = cos(newYaw) * -width + cos(newPitch) * dist * sin(newYaw);
	crntPos.y = sin(newPitch) * dist;
	crntPos.z = sin(newYaw) * width + cos(newPitch) * dist * cos(newYaw);
	
	// Assigns the normal from the Vertex Data to "Normal"
	Normal = vec3(0.0, 1.0, 0.0);
	// Assigns the colors from the Vertex Data to "color"
	color = vec3(0.0, 0.0, 0.0);
	// Assigns the texture coordinates from the Vertex Data to "texCoord"

	float tex_XCoord = floor((curr_vertex) / 2.0);
	if (curr_vertex > 1) {tex_XCoord = 0.5;}
	texCoord = mat2(0.0, -1.0, 1.0, 0.0) * vec2( tex_XCoord, width + 0.5 );
	
	// Outputs the positions/coordinates of all vertices
	gl_Position = camMatrix * vec4(crntPos + aPos, 1.0);
}