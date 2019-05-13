//
// Fragment Shader für Sharpen Operator
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


uniform vec3 filter[5] = vec3[](	vec3( 0,  0,  4),
									vec3(-1,  0, -1),
									vec3( 1,  0, -1),
									vec3( 0, -1, -1),
									vec3( 0,  1, -1)
									);

void main()
{

	vec4 filterValue = vec4(0, 0, 0, 0);
	for (int i = 0; i < filter.length(); i++) {
		filterValue += filter[i].z * texture(textureMap, texCoords + filter[i].xy);
	}

	fragColor = texture(textureMap, texCoords) + param1.z * 0.1 * filterValue;
}