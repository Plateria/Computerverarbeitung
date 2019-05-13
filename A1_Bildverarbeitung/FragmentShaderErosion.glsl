//
// Fragment Shader für den morphologischen Operator 
// Erosion
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


void main()
{

	fragColor = vec4(1, 1, 1, 1);

	int distance = min(3, max(1, int(param1.w) + 1));
	for (int x = -distance; x <= distance; x++) {
		for (int y = -distance; y <= distance; y++) {
			fragColor = min(fragColor, texture(textureMap, texCoords + vec2(x, y)));
		}
	}
	
}