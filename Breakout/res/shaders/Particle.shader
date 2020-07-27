#shader vertex
#version 330 core
layout(location = 0) in vec4 vertex;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 projection;
uniform vec2 offset;
uniform vec4 color;

void main()
{
	float scale = 10.0f;
	TexCoords = vertex.zw;
	ParticleColor = color;
	gl_Position = projection * vec4((vertex.xy * scale) + offset, 0.0, 1.0);
}

#shader fragment
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in vec4 ParticleColor;

uniform sampler2D sprite;

void main()
{
	FragColor = (texture(sprite, TexCoords) * ParticleColor);
}