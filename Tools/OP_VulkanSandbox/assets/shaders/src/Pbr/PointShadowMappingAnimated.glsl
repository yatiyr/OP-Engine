#type vertex
#version 450 core

// ------------------ DEFINES ----------------- //
#include Defines.glsl
// -------------------------------------------- //

layout(location = 0) in vec3 a_Position;
layout (location = 5) in ivec4 a_BoneIDs;
layout (location = 6) in vec4  a_BoneWeights;

// -------------- UNIFORM BUFFERS ------------- //
#include UniformBuffers.glsl
// -------------------------------------------- //

void main()
{
	vec4 totalPosition = vec4(0.0);
	int counter = 0;

    mat4 BoneTransform = u_BoneMatrices[a_BoneIDs[0]].mat * a_BoneWeights[0];
		BoneTransform += u_BoneMatrices[a_BoneIDs[1]].mat * a_BoneWeights[1];
		BoneTransform += u_BoneMatrices[a_BoneIDs[2]].mat * a_BoneWeights[2];
		BoneTransform += u_BoneMatrices[a_BoneIDs[3]].mat * a_BoneWeights[3];

	
	//if(a_BoneWeights[0] == 0 && a_BoneWeights[1] == 0 && a_BoneWeights[2] == 0 && a_BoneWeights[3] == 0)
		//BoneTransform = mat4(1.0);

	totalPosition = BoneTransform * vec4(a_Position, 1.0);

	gl_Position = u_Model * totalPosition;
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