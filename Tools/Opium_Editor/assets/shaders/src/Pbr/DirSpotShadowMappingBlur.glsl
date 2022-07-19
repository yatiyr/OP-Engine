#type vertex
#version 450 core

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

#type geometry
#version 450 core


#define MAX_DIR_LIGHTS 4
#define MAX_SPOT_LIGHTS 4
#define MAX_CASCADE_SIZE 10

layout (triangles, invocations=MAX_DIR_LIGHTS + MAX_SPOT_LIGHTS) in;
layout (triangle_strip, max_vertices = 3 * MAX_CASCADE_SIZE) out;

struct VS_OUT
{
	vec2 TexCoords;
};

layout (location = 0) in VS_OUT fs_in[3];

struct GS_OUT
{
	vec2 TexCoords;
};

layout (location = 1) out GS_OUT gs_out;

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


void main()
{
	int invocationID = gl_InvocationID;

	// This means that we are processing directional lights
	if(invocationID < MAX_DIR_LIGHTS)
	{
		for(int i=0; i<u_DirLights[invocationID].CascadeSize; i++)
		{
			gl_Layer = invocationID * MAX_CASCADE_SIZE + i;
			for(int k=0; k<3; k++)
			{
				gl_Position = gl_in[k].gl_Position;
				gs_out.TexCoords = fs_in[k].TexCoords;
				EmitVertex();
			}
			EndPrimitive();
		}

	}
	// We are processing spot lights
	else if(invocationID >= MAX_DIR_LIGHTS)
	{
		int index = invocationID - MAX_DIR_LIGHTS;
		gl_Layer = MAX_DIR_LIGHTS * MAX_CASCADE_SIZE + index;
		for(int k=0; k<3; k++)
		{
			gl_Position = gl_in[k].gl_Position;
			gs_out.TexCoords = fs_in[k].TexCoords;
			EmitVertex();
		}
		EndPrimitive();
	}
}


#type fragment
#version 450 core

layout(location = 0) out vec4 FragColor;

struct GS_OUT
{
	vec2 TexCoords;
};

layout (location = 1) in GS_OUT gs_in;

layout (binding = 0) uniform sampler2DArray u_ShadowMapDirSpot;

layout(std140, binding = 2) uniform ShadowMapSettings
{
	float u_ShadowMapResX;
	float u_ShadowMapResY;
	vec2 u_BlurScale;
};

void main()
{
	vec4 color = vec4(0.0);
	vec2 texCoords = gs_in.TexCoords;

	color += texture(u_ShadowMapDirSpot, vec3(texCoords + (vec2(-3.0) * u_BlurScale) , gl_Layer)) * (1.0/64.0);
	color += texture(u_ShadowMapDirSpot, vec3(texCoords + (vec2(-2.0) * u_BlurScale) , gl_Layer)) * (6.0/64.0);
	color += texture(u_ShadowMapDirSpot, vec3(texCoords + (vec2(-1.0) * u_BlurScale) , gl_Layer)) * (15.0/64.0);
	color += texture(u_ShadowMapDirSpot, vec3(texCoords + (vec2( 0.0) * u_BlurScale) , gl_Layer)) * (20.0/64.0);
	color += texture(u_ShadowMapDirSpot, vec3(texCoords + (vec2( 1.0) * u_BlurScale) , gl_Layer)) * (15.0/64.0);
	color += texture(u_ShadowMapDirSpot, vec3(texCoords + (vec2( 2.0) * u_BlurScale) , gl_Layer)) * (6.0/64.0);
	color += texture(u_ShadowMapDirSpot, vec3(texCoords + (vec2( 3.0) * u_BlurScale) , gl_Layer)) * (1.0/64.0);

	FragColor = color;
}