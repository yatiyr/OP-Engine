#type vertex
#version 450 core

// ------------------ DEFINES ----------------- //
#include Defines.glsl
// -------------------------------------------- //

layout(location = 0) in vec3 a_Position;

struct VS_OUT
{
	vec3 LocalPosition;
};

// -------------- UNIFORM BUFFERS ----------- //
#include UniformBuffers.glsl
// ------------------------------------------ //

layout (location = 0) out VS_OUT vs_out;

void main()
{

	vs_out.LocalPosition = a_Position;

	// Remove translation from u_View
	mat4 viewWithoutTranslation = mat4(mat3(u_View));
	vec4 pos = u_Projection *  viewWithoutTranslation * vec4(a_Position, 1.0);

	gl_Position = pos.xyww;
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
	vec3 LocalPosition;
};

layout (binding = 0) uniform samplerCube u_EnvironmentMap;

layout (location = 0) in VS_OUT fs_in;

// -------------------- UTILS ----------------- //
#include Utils.glsl
// -------------------------------------------- //

void main()
{

	vec3 envColor = textureLod(u_EnvironmentMap, fs_in.LocalPosition, 0.0).rgb;

	FragColor = vec4(envColor, 1.0);
}