//
// Fragment Shader für 7x7 Gauss Tiefpassfilter
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

uniform vec2 offsets[7] = vec2[]( vec2(0, 3), vec2(0, 2), vec2(0, 1), vec2(0, 0), vec2(0, -1), vec2(0, -2), vec2(0, -3)	);

//uniform vec2 offsets[49] = vec2[](		vec2(-3,  3), vec2(-2,  3), vec2(-1,  3), vec2(0,  3), vec2(1,  3), vec2(2,  3), vec2(3,  3), 
//										vec2(-3,  2), vec2(-2,  2), vec2(-1,  2), vec2(0,  2), vec2(1,  2), vec2(2,  2), vec2(3,  2),
//										vec2(-3,  1), vec2(-2,  1), vec2(-1,  1), vec2(0,  1), vec2(1,  1), vec2(2,  1), vec2(3,  1),
//										vec2(-3,  0), vec2(-2,  0), vec2(-1,  0), vec2(0,  0), vec2(1,  0), vec2(2,  0), vec2(3,  0),
//										vec2(-3, -1), vec2(-2, -1), vec2(-1, -1), vec2(0, -1), vec2(1, -1), vec2(2, -1), vec2(3, -1),
//										vec2(-3, -2), vec2(-2, -2), vec2(-1, -2), vec2(0, -2), vec2(1, -2), vec2(2, -2), vec2(3, -2),
//										vec2(-3, -3), vec2(-2, -3), vec2(-1, -3), vec2(0, -3), vec2(1, -3), vec2(2, -3), vec2(3, -3)	);

void main()
{
	vec4 result = vec4(0.0, 0.0, 0.0, 1.0);

	float partials = 0;
	float variance = 2 * param1.z + 1;
	float PI = 3.1415926;

	for (int i = 0; i < offsets.length(); i++)
	{
		vec2 coords = offsets[i];
		vec2 coords2 = coords * coords;
		float tmp = coords2.x + coords2.y;
		tmp = tmp * (-1.0);
		tmp = tmp / variance;
		tmp = exp(tmp);
		tmp = tmp / (PI * variance);
		result += tmp * texture(textureMap, texCoords + offsets[i]);
		partials += tmp;
	}
	fragColor = result / partials;
}