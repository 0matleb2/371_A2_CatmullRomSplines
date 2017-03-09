#version 330 core

in vec4 vPos;

out vec4 color;

void main()
{
	color = vec4(pow((vPos.x + 1)/2,2), pow((vPos.y + 1) / 2, 2), pow((vPos.z + 1) / 2, 2), 1.0f);
} 