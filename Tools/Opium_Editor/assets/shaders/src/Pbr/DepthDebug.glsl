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

// -------------- UNIFORM BUFFERS ----------- //
#include UniformBuffers.glsl
// ------------------------------------------ //

layout (location = 0) out VS_OUT vs_out;

void main()
{
	vs_out.TexCoords = a_TexCoords;
	gl_Position = u_Model * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

// ------------------ DEFINES ----------------- //
#include Defines.glsl
// -------------------------------------------- //

// -------------- UNIFORM BUFFERS ----------- //
#include UniformBuffers.glsl
// ------------------------------------------ //

layout(location = 0) out vec4 FragColor;

struct VS_OUT
{
	vec2 TexCoords;
};

layout (binding = 1) uniform sampler2D u_ShadowMapDirSpot;

layout (location = 0) in VS_OUT fs_in;

// -------------------- UTILS ----------------- //
#include Utils.glsl
// -------------------------------------------- //

void main()
{
    vec3 val = texture(u_ShadowMapDirSpot, fs_in.TexCoords).xyz;
	FragColor = vec4(val, 1.0);
}