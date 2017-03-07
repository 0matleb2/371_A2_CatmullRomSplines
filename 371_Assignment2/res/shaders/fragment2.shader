#version 330 core

in vec4 vPos;

out vec4 color;

void main()
{
	color = vec4(vPos.x, vPos.y, vPos.z, 1.0f);
} 