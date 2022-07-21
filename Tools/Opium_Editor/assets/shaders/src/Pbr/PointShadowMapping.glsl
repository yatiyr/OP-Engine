#type vertex
#version 450 core

// ------------------ DEFINES ----------------- //
#include Defines.glsl
// -------------------------------------------- //

layout (location = 0) in vec3 a_Position;

#include UniformBuffers.glsl

void main()
{
	gl_Position = u_Model * vec4(a_Position, 1.0);
}

#type geometry
#version 450 core

// ------------------ DEFINES ----------------- //
#include Defines.glsl
// -------------------------------------------- //

layout (triangles, invocations = MAX_POINT_LIGHTS) in;
layout (triangle_strip, max_vertices = 18) out;

// ---------------- UNIFORM BUFFERS ------------- //
#include UniformBuffers.glsl
// ---------------------------------------------- //

struct GS_OUT
{
	vec4 FragPos;
};

layout (location = 1) out GS_OUT gs_out;

void main()
{
	int invocationID = gl_InvocationID;

	for(int i=0; i<6; i++)
	{
		gl_Layer = invocationID * 6 + i;
		mat4 lightSpaceMatrix = u_LightSpaceMatricesPoint[invocationID * 6 + i].mat;
		for(int k=0; k<3; k++)
		{
			gs_out.FragPos = gl_in[k].gl_Position;
			gl_Position = lightSpaceMatrix * gl_in[k].gl_Position;
			EmitVertex();
		}
		EndPrimitive();
	}
	
}

#type fragment
#version 450 core

struct GS_OUT
{
	vec4 FragPos;
};

layout (location = 1) in GS_OUT fs_in;

#include Defines.glsl
#include UniformBuffers.glsl

void main()
{
	int lightIndex = gl_Layer / 6;

	float dist = length(fs_in.FragPos.xyz - u_PointLights[lightIndex].Position); 

	dist = dist / u_PointLights[lightIndex].FarDist;

	gl_FragDepth = dist;
}