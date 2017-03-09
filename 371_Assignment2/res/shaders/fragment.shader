#version 330 core

in vec3 vPos;

out vec4 color;

void main()
{
	color = vec4( (vPos.x + 1) / 2, (vPos.y + 1) / 2, (vPos.z + 1) / 2, 1.0f );
} 