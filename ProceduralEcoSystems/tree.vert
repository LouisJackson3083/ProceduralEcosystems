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
uniform int resolution;
uniform int segments;
uniform float height;
uniform float radius;
uniform float radius_falloff;
uniform float radius_falloff_rate;
uniform int vertices_per_trunk;

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

	int curr_vertex = int(modI(gl_VertexID, vertices_per_trunk));
	int curr_circle_index = int(modI(gl_VertexID, resolution));
	int curr_tree = int(floor(gl_VertexID / vertices_per_trunk));

	float t = 6.282 * curr_circle_index / resolution;

	float curr_segment = floor(curr_vertex / resolution);

	float segment_radius = radius / (radius_falloff * pow(curr_segment+1, radius_falloff_rate));

	crntPos.x = segment_radius * cos(t);
	crntPos.y = height * (curr_segment / (segments - 1));
	crntPos.z = segment_radius * sin(t);

	//crntPos.x = radius * cos(6.28318530717958647692 * modI(curr_vertex, resolution) / float(resolution));
	//crntPos.y = height * floor(float(curr_vertex)/float(resolution));
	//crntPos.z = radius * sin(6.28318530717958647692 * modI(curr_vertex, resolution) / float(resolution));

	// Assigns the normal from the Vertex Data to "Normal"
	Normal = vec3(0.0, 1.0, 0.0);
	// Assigns the colors from the Vertex Data to "color"
	color = vec3(0.0, 0.0, 0.0);
	// Assigns the texture coordinates from the Vertex Data to "texCoord"
	texCoord = mat2(0.0, -1.0, 1.0, 0.0) * vec2(crntPos.y, curr_circle_index);
	
	// Outputs the positions/coordinates of all vertices
	gl_Position = camMatrix * vec4(aPos + crntPos, 1.0);
}