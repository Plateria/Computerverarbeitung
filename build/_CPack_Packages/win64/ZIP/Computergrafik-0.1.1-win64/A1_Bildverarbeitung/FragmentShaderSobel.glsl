//
// Fragment Shader für Sobel Operator
// Angepasst für Core Profile
// ---------------------------------
//
// @author: Prof. Dr. Alfred Nischwitz
// @lecturer: Prof. Dr. Alfred Nischwitz
//
// (c)2017 Hochschule München, HM
//
// ---------------------------------
#version 330


smooth in vec2 texCoords;				// pixelbezogene Texturkoordinate
out vec4 fragColor;					// Ausgabewert mit 4 Komponenten zwischen 0.0 und 1.0
uniform sampler2DRect textureMap;		// Sampler für die Texture Map
uniform vec4 param1;					// Param1 X,Y,Z,W in GUI


uniform vec3 horizontalFilter[6] = vec3[](	vec3(-1, -1,  1),
											vec3(-1,  0,  2),
											vec3(-1,  1,  1),
											vec3( 1, -1, -1),
											vec3( 1,  0, -2),
											vec3( 1,  1, -1)
											);

uniform vec3 verticalFilter[6] = vec3[](	vec3(-1, -1,  1),
											vec3( 0, -1,  2),
											vec3( 1, -1,  1),
											vec3(-1,  1, -1),
											vec3( 0,  1, -2),
											vec3( 1,  1, -1)
											);

void main()
{

	vec4 hValue = vec4(0, 0, 0, 0);
	vec4 vValue = vec4(0, 0, 0, 0);

	for (int i = 0; i < horizontalFilter.length(); i++) {
		hValue += horizontalFilter[i].z * texture(textureMap, texCoords + horizontalFilter[i].xy);
	}

	for (int i = 0; i < verticalFilter.length(); i++) {
		vValue += verticalFilter[i].z * texture(textureMap, texCoords + verticalFilter[i].xy);
	}

	fragColor = vec4(length(vec2(hValue.x, vValue.x)),
						length(vec2(hValue.y, vValue.y)),
						length(vec2(hValue.z, vValue.z)),
						length(vec2(hValue.w, vValue.w)));

	fragColor += param1.x * 0.01;
	fragColor = ((fragColor - 0.5) * (1 + param1.y * 0.01)) + 0.5;
}