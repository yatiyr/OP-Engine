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
#define MAX_CASCADE_SIZE 10

layout (triangles, invocations=MAX_DIR_LIGHTS + MAX_SPOT_LIGHTS) in;
layout (triangle_strip, max_vertices = 3 * MAX_CASCADE_SIZE) out;

// Directional Light Data
struct DirLight
{ 
	int CascadeSize;
	float FrustaDistFactor;
	vec3 LightDir;
	vec3 Color;
};

layout(std140, binding = 3) uniform DirLightData
{
	int u_DirLightSize;
	DirLight u_DirLights[MAX_DIR_LIGHTS];
};

// Spot Light Data
struct SpotLight
{
	float Cutoff;
	float OuterCutoff;
	float NearDist;
	float FarDist;
	float Bias;
	float Kq;
	float Kl;
	vec3 LightDir;
	vec3 Color;
	vec3 Position;
};

layout(std140, binding = 4) uniform SpotLightData
{
	int u_SpotLightSize;
	SpotLight u_SpotLights[MAX_SPOT_LIGHTS];
};

struct LightSpaceMat
{
	mat4 mat;
};

layout(std140, binding = 5) uniform LightSpaceMatricesDSData
{
	LightSpaceMat u_LightSpaceMatricesDirSpot[MAX_DIR_LIGHTS * MAX_CASCADE_SIZE + MAX_SPOT_LIGHTS];
};

struct GS_OUT
{
	vec4 FragPos;
};

layout (location = 1) out GS_OUT gs_out;

void main()
{
	int invocationID = gl_InvocationID;

	// This means that we are processing directional lights
	if(invocationID < MAX_DIR_LIGHTS)
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
	else if(invocationID >= MAX_DIR_LIGHTS)
	{
		int index = invocationID - MAX_DIR_LIGHTS;
		gl_Layer = MAX_DIR_LIGHTS * MAX_CASCADE_SIZE + index;
		mat4 lightSpaceMatrix = u_LightSpaceMatricesDirSpot[gl_Layer].mat;
		for (int k=0; k<3; k++)
		{
			gs_out.FragPos = gl_in[k].gl_Position;
		    gl_Position = lightSpaceMatrix * gs_out.FragPos;
			EmitVertex();
		}
		EndPrimitive();
	}
}


#type fragment
#version 450 core

#define MAX_DIR_LIGHTS 4
#define MAX_SPOT_LIGHTS 4
#define MAX_CASCADE_SIZE 10

layout(location = 0) out vec4 FragColor;

struct GS_OUT
{
	vec4 FragPos;
};

layout (location = 1) in GS_OUT fs_in;


// Spot Light Data
struct SpotLight
{
	float Cutoff;
	float OuterCutoff;
	float NearDist;
	float FarDist;
	float Bias;
	float Kq;
	float Kl;
	vec3 LightDir;
	vec3 Color;
	vec3 Position;
};

layout(std140, binding = 4) uniform SpotLightData
{
	int u_SpotLightSize;
	SpotLight u_SpotLights[MAX_SPOT_LIGHTS];
};

float PerspectiveProjDepthLinearize(float depth, float near, float far)
{
	float z = depth * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{

	if(gl_Layer < MAX_CASCADE_SIZE * MAX_DIR_LIGHTS)
	{
		float depth = gl_FragCoord.z;
		float dx = dFdx(depth);
		float dy = dFdy(depth);
		float moment2 = depth * depth + 0.25 * (dx*dx + dy*dy);

		FragColor = vec4(depth, moment2, 1.0, 1.0);
	}
	else
	{
		/*float depth = gl_FragCoord.z;
		int index = gl_Layer - MAX_CASCADE_SIZE * MAX_DIR_LIGHTS;
		float near = u_SpotLights[index].NearDist;
		float far = u_SpotLights[index].FarDist;

		depth = PerspectiveProjDepthLinearize(depth, near, far) / far;
		float dx = dFdx(depth);
		float dy = dFdy(depth);
		float moment2 = depth * depth + 0.25 * (dx*dx + dy*dy); */

		int index = gl_Layer - MAX_CASCADE_SIZE * MAX_DIR_LIGHTS;
		float dist = length(fs_in.FragPos.xyz - u_SpotLights[index].Position); 

		float dist2 = dist / u_SpotLights[index].FarDist;

		float depth = dist2;
		float dx = dFdx(depth);
		float dy = dFdy(depth);
		float moment2 = depth * depth + 0.25 * (dx*dx + dy*dy);
		
		FragColor = vec4(depth, moment2, 1.0, 1.0); 
	}
	

}