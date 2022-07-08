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

layout (triangles) in;
layout (triangle_strip, max_vertices = 3 * (MAX_DIR_LIGHTS + MAX_SPOT_LIGHTS)) out;

// Directional Light Data
struct DirLight
{
	mat4 LightSpaceMatrix;
	vec3 LightDir;
	vec3 Color;
};

layout(std140, binding = 2) uniform DirLightData
{
	int u_Size;
	DirLight u_DirLights[MAX_DIR_LIGHTS];
};


void main()
{
	for(int dirLight = 0; dirLight < MAX_DIR_LIGHTS + MAX_SPOT_LIGHTS; dirLight++)
	{
		gl_Layer = dirLight;
		for(int i=0; i<3; i++)
		{
			gl_Position    = u_DirLights[dirLight].LightSpaceMatrix * gl_in[i].gl_Position;
			EmitVertex();
		}
		EndPrimitive();
	}
}


#type fragment
#version 450 core

void main()
{
	gl_FragDepth = gl_FragCoord.z;
}