
#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

// Imports the current position from the Vertex Shader
in vec3 crntPos;
// Imports the normal from the Vertex Shader
in vec3 Normal;
// Imports the color from the Vertex Shader
in vec3 color;
// Imports the texture coordinates from the Vertex Shader
in vec2 texCoord;

// Gets the Texture Units from the main function
uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform sampler2D diffuse1;
uniform sampler2D specular1;
uniform sampler2D diffuse2;
uniform sampler2D specular2;
// Gets the color of the light from the main function
uniform vec4 lightColor;
// Gets the position of the light from the main function
uniform vec3 lightPos;
// Gets the position of the camera from the main function
uniform vec3 camPos;

uniform float snow_blend_distance;
uniform float snow_height;
uniform float rock_blend_distance;
uniform float rock_height;
uniform float slope_amount;
uniform float slope_blend_distance;


vec4 pointLight()
{	
	// used in two variables so I calculate it here to not have to do it twice
	vec3 lightVec = lightPos - crntPos;

	// intensity of light with respect to distance
	float dist = length(lightVec);
	float a = 3.0;
	float b = 0.7;
	float inten = 1.0f / (a * dist * dist + b * dist + 1.0f);

	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return (texture(diffuse0, texCoord) * (diffuse * inten + ambient) + texture(specular0, texCoord).r * specular * inten) * lightColor;
}

vec4 direcLight()
{
	// ambient lighting
	float ambient = 0.50f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(vec3(1.0f, 1.0f, -2.0f));
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.20f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	float pitch = asin(-normal.y) / 1.57079632679;
	float snow_start_height = snow_height - snow_blend_distance;
	float rock_start_height = rock_height - rock_blend_distance;

	// diffuse2 = dirt
	// diffuse1 = rock
	// diffuse0 = snow
	vec4 diffuse_tex = texture(diffuse2, texCoord);
	vec4 specular_tex = texture(diffuse2, texCoord);
	// Handle rock blending
	if ( crntPos.y > rock_height ) {
		diffuse_tex = texture(diffuse1, texCoord);
		specular_tex = texture(specular1, texCoord);
	}
	else if ( crntPos.y > rock_start_height ) {
		diffuse_tex = mix(texture(diffuse1, texCoord), diffuse_tex, abs(crntPos.y - rock_height)/rock_blend_distance);
		specular_tex = mix(texture(specular1, texCoord), specular_tex, abs(crntPos.y - rock_height)/rock_blend_distance);
	}
	
	// Handle slope stuff
	if (normal.y < slope_amount) {
		diffuse_tex = texture(diffuse1, texCoord);
		specular_tex = texture(diffuse1, texCoord);
	}
	else if (normal.y < slope_amount + slope_blend_distance) {
		diffuse_tex = mix(texture(diffuse1, texCoord), diffuse_tex, abs(normal.y - slope_amount)/slope_blend_distance);
		specular_tex = mix(texture(specular1, texCoord), specular_tex, abs(normal.y - slope_amount)/slope_blend_distance);
	}

	// Handle snow blending
	if ( crntPos.y > snow_height ) {
		diffuse_tex = texture(diffuse0, texCoord);
		specular_tex = texture(specular1, texCoord);
	}
	else if ( crntPos.y > snow_start_height ) {
		diffuse_tex = mix(texture(diffuse0, texCoord), diffuse_tex, abs(crntPos.y - snow_height)/snow_blend_distance);
		specular_tex = mix(texture(specular0, texCoord), specular_tex, abs(crntPos.y - snow_height)/snow_blend_distance);
	}


	return (diffuse_tex * (diffuse + ambient) + specular_tex.r * specular) * lightColor;
}

vec4 spotLight()
{
	// controls how big the area that is lit up is
	float outerCone = 0.90f;
	float innerCone = 0.95f;

	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPos - crntPos);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	// calculates the intensity of the crntPos based on its angle to the center of the light cone
	float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDirection);
	float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

	return (texture(diffuse0, texCoord) * (diffuse * inten + ambient) + texture(specular0, texCoord).r * specular * inten) * lightColor;
}

 
float near = 0.001; 
float far  = 100.0; 

float linearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}


void main()
{
	// outputs final color
	//FragColor = direcLight();
	float depth = linearizeDepth(gl_FragCoord.z);
	FragColor = direcLight() * (1.0f - depth) + vec4(depth * vec3(0.85f, 0.85f, 0.90f), 1.0f);

    //float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
    //FragColor = 1.0 - vec4(vec3(depth), 1.0) * direcLight();
}