#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;

void main()
{
	gl_Position = vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 color;

void main()
{
	color = vec4(1.0,1.0,1.0,1.0);
}