#shader vertex
#version 330 core
layout(location = 0) in vec4 vertex;

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 model;

void main()
{
	TexCoords = vertex.zw;
	gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
}

#shader fragment
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D image;
uniform vec3 spriteColor;

void main()
{
	FragColor = vec4(spriteColor, 1.0) * texture(image, TexCoords);
}