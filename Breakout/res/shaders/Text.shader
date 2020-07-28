#shader vertex
#version 330 core
layout(location = 0) in vec4 vertex;
out vec2 TexCoords;

uniform mat4 projection;

void main()
{
	TexCoords = vertex.zw;
	gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
}

#shader fragment
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D text;
uniform vec3 color;

void main()
{
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
	FragColor = vec4(color, 1.0) * sampled;
}