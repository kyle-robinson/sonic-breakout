#shader vertex
#version 330 core
layout(location = 0) in vec4 vertex;

out vec2 TexCoords;

uniform bool chaos;
uniform bool confuse;
uniform bool shake;
uniform bool circle;
uniform float time;

void main()
{
	gl_Position = vec4(vertex.xy, 0.0f, 1.0f);
	vec2 texture = vertex.zw;

	if (chaos)
	{
		float strength = 0.3;
		vec2 pos = vec2(texture.x + sin(time) * strength, texture.y + cos(time) * strength);
		TexCoords = pos;
	}
	else if (confuse)
	{
		TexCoords = vec2(1.0 - texture.x, 1.0 - texture.y);
	}
	else if (circle)
	{
		float strength = 0.05;
		vec2 pos = vec2(texture.x + sin(time) * strength / 2, texture.y + cos(time) * strength / 2);
		TexCoords = pos;
	}
	else
	{
		TexCoords = texture;
	}
	
	if (shake)
	{
		float strength = 0.01;
		gl_Position.x += cos(time * 10) * strength;
		gl_Position.y += cos(time * 15) * strength;
	}

}

#shader fragment
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D scene;
uniform vec2 offsets[9];
uniform int edge_kernel[9];
uniform float blur_kernel[9];

uniform bool chaos;
uniform bool confuse;
uniform bool shake;
uniform bool darken;

void main()
{
	FragColor = vec4(0.0f);
	vec3 sample[9];

	// sample from texture offsets (if using convolution matrix)
	if (chaos || shake)
		for (int i = 0; i < 9; i++)
			sample[i] = vec3(texture(scene, TexCoords.st + offsets[i]));

	if (chaos)
	{
		for (int i = 0; i < 9; i++)
			FragColor += vec4(sample[i] * edge_kernel[i], 0.0f);
		FragColor.a = 1.0f;
	}
	else if (confuse)
	{
		FragColor = vec4(1.0 - texture(scene, TexCoords).rgb, 1.0);
	}
	else if (shake)
	{
		for (int i = 0; i < 9; i++)
			FragColor += vec4(sample[i] * blur_kernel[i], 0.0f);
		FragColor.a = 1.0f;
	}
	else if (darken)
	{
		FragColor = texture(scene, TexCoords) * 0.4f;
	}
	else
	{
		FragColor = texture(scene, TexCoords);
	}
}