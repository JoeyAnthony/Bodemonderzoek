#version 150

uniform sampler2D s_mixmap;
uniform sampler2D s_texture0;
uniform sampler2D s_texture1;
uniform vec3 diffuseColor;
uniform float textureFactor;

in vec2 texCoord;
in vec3 normal;
out vec4 fragColor;

void main()
{
	float diffuse = dot(normalize(vec3(1,1,1)), normalize(normal));
	diffuse = clamp(diffuse, 0, 1);
	float light = 0.5 * diffuse + 0.5;
	
	// Get the color information
	vec3 mixmap    = texture2D(s_mixmap, texCoord / 40).rgb;
	vec3 texGrass  = texture2D(s_texture0, texCoord).rgb;
	vec3 texAsphalt = texture2D(s_texture1,  texCoord).rgb;

	texAsphalt *= mixmap.r;
	vec3 tx = mix(texAsphalt,  texGrass, mixmap.g);
	//texWater = mix(texGrass, texWater, mixmap.b);
	//vec3 tx  = mix(texWater, texRock,  mixmap.a);

	vec3 tex = mix(diffuseColor, tx, textureFactor);

	fragColor.rgb = light * tex;
	fragColor.a = 1.0;
}
