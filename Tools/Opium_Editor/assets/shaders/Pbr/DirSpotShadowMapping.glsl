#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;

layout(std140, binding = 1)  uniform Transform
{
	mat4 u_Model;
};

void main()
{
	gl_Position = u_Model * vec4(a_Position, 1.0);
}

#type geometry
#version 450 core


#define MAX_DIR_LIGHTS 4
#define MAX_SPOT_LIGHTS 4
#define MAX_CASCADE_SIZE 20

layout (triangles, invocations=MAX_DIR_LIGHTS + MAX_SPOT_LIGHTS) in;
layout (triangle_strip, max_vertices = 3 * MAX_CASCADE_SIZE) out;

// Directional Light Data
struct DirLight
{ 
	int CascadeSize;
	int FrustaDistFactor;
	vec3 LightDir;
	vec3 Color;
};

layout(std140, binding = 2) uniform DirLightData
{
	int u_DirLightSize;
	DirLight u_DirLights[MAX_DIR_LIGHTS];
};

struct LightSpaceMat
{
	mat4 mat;
};

layout(std140, binding = 3) uniform LightSpaceMatricesDSData
{
	LightSpaceMat u_LightSpaceMatricesDirSpot[MAX_DIR_LIGHTS * MAX_CASCADE_SIZE + MAX_SPOT_LIGHTS];
};

void main()
{
	int invocationID = gl_InvocationID;

	// This means that we are processing directional lights
	if(invocationID < u_DirLightSize)
	{
		for(int i=0; i<u_DirLights[invocationID].CascadeSize; i++)
		{
			gl_Layer = invocationID * MAX_CASCADE_SIZE + i;
			mat4 lightSpaceMatrix = u_LightSpaceMatricesDirSpot[invocationID * MAX_CASCADE_SIZE + i].mat;
			for(int k=0; k<3; k++)
			{
				gl_Position = lightSpaceMatrix * gl_in[k].gl_Position;
				EmitVertex();
			}
			EndPrimitive();
		}
		/*
		gl_Layer = invocationID;
		int dirLightIndex   = invocationID / MAX_CASCADE_SIZE;
		int lightSpaceIndex = dirLightIndex * MAX_CASCADE_SIZE;
		int cascadeIndex    = invocationID - lightSpaceIndex;
		if(cascadeIndex < u_DirLights[dirLightIndex].CascadeSize)
		{
			mat4 lightSpaceMatrix = u_LightSpaceMatricesDirSpot[invocationID].mat;
			for(int i=0; i<3; i++)
			{
				gl_Position = lightSpaceMatrix * gl_in[i].gl_Position;
				EmitVertex();
			}
			EndPrimitive();
		}*/

	}
	// We are processing spot lights
	else if(invocationID >= MAX_DIR_LIGHTS * MAX_CASCADE_SIZE)
	{
		// TODO: WILL BE IMPLEMENTED SOON
	}
}


#type fragment
#version 450 core

void main()
{
 // pass through shader, we do not need to do anything.
}