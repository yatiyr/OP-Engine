#type vertex
#version 450 core

// ------------------ DEFINES ----------------- //
#include Defines.glsl
// -------------------------------------------- //

layout(location = 0) in vec3 a_Position;
layout(location = 2) in vec2 a_TexCoords;

struct VS_OUT
{
	vec2 TexCoords;
};

layout (location = 0) out VS_OUT vs_out;

void main()
{
	vs_out.TexCoords = a_TexCoords;
	gl_Position =  vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 FragColor;

struct VS_OUT
{
	vec2 TexCoords;
};

layout (location = 0) in VS_OUT fs_in;

layout (binding = 0) uniform sampler2D u_Image;

// ------------------ DEFINES ----------------- //
#include Defines.glsl
// -------------------------------------------- //

// -------------- UNIFORM BUFFERS ------------- //
#include UniformBuffers.glsl
// -------------------------------------------- //

void main()
{
	vec4 color = texture(u_Image, fs_in.TexCoords);
	FragColor = color;
}