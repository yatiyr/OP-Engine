#type vertex
#version 450 core

// ------------------ DEFINES ----------------- //
#include Defines.glsl
// -------------------------------------------- //

layout (location = 0) in vec3  a_Position;
layout (location = 1) in vec3  a_Normal;
layout (location = 2) in vec2  a_TexCoords;
layout (location = 3) in vec3  a_Tangent;
layout (location = 4) in vec3  a_Bitangent;
layout (location = 5) in ivec4 a_BoneIDs;
layout (location = 6) in vec4  a_BoneWeights;


// -------------- UNIFORM BUFFERS ------------- //
#include UniformBuffers.glsl
// -------------------------------------------- //


void main()
{
	gl_Position =  u_ViewProjection * u_Model * vec4(a_Position + a_Normal * 0.02, 1.0);
}

#type fragment
#version 450 core

// ------------------ DEFINES ----------------- //
#include Defines.glsl
// -------------------------------------------- //


layout(location = 0) out vec4 FragColor;


layout (location = 0) uniform vec3 u_OutlineColor;


// ------------- GLOBAL VARIABLES ------------- //
#include GlobalVariables.glsl
// -------------- UNIFORM BUFFERS ------------- //
#include UniformBuffers.glsl
// ----------------- UTILS -------------------- //
#include Utils.glsl

void main()
{
	FragColor = vec4(u_OutlineColor, 1.0);
}