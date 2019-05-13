//
// Fragment Shader für Laplace Operator
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

uniform vec2 offsets[9] = vec2[](	vec2(-1,  1),
									vec2(-1,  0),
									vec2(-1, -1),
									vec2( 0,  1),
									vec2( 0,  0),
									vec2( 0, -1),
									vec2( 1,  1),
									vec2( 1,  0),
									vec2( 1, -1)	);

uniform float laplace[9] = float[]( 0, -1, 0,
									-1, 4, -1,
									0, -1, 0);

void main()
{
	vec4 result = vec4(0.0, 0.0, 0.0, 1.0);

	for (int i = 0; i < laplace.length(); i++) {
		result += laplace[i] * texture(textureMap, texCoords + offsets[i]);
	}
	fragColor = 0.5 + result;
	fragColor = fragColor + (param1.x * 0.01);
	fragColor -= 0.5;
	fragColor = fragColor * (1 + (param1.y * 0.01));
	fragColor += 0.5;

}