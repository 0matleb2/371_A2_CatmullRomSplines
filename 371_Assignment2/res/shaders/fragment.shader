#version 330 core

in vec2 vPosition;

out vec4 color;

void main()
{
	color = vec4(vPosition.x, vPosition.y, 1.0f, 1.0f);
} 