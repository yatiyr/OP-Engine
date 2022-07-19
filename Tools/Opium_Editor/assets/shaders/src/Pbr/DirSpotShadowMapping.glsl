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


// ------------------ DEFINES ----------------- //
#include Defines.glsl
// -------------------------------------------- //

layout (triangles, invocations=MAX_DIR_LIGHTS + MAX_SPOT_LIGHTS) in;
layout (triangle_strip, max_vertices = 3 * MAX_CASCADE_SIZE) out;

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

// ------------------ DEFINES ----------------- //
#include Defines.glsl
// -------------------------------------------- //

layout(location = 0) out vec4 FragColor;

struct GS_OUT
{
	vec4 FragPos;
};

layout (location = 1) in GS_OUT fs_in;

// ---------------- UNIFORM BUFFERS ------------- //
#include UniformBuffers.glsl
// ---------------------------------------------- //

// ------------------- UTILS -------------------- //
#include Utils.glsl
// ---------------------------------------------- //

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