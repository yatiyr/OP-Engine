#type vertex
#version 450 core

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
layout (triangle_strip, max_vertices = 3) out;

// ---------------- UNIFORM BUFFERS ------------- //
#include UniformBuffers.glsl
// ---------------------------------------------- //


void main()
{
	int invocationID = gl_InvocationID;

	for(int i=0; i<6; i++)
	{
		gl_Layer = invocationID * 6 + i;
		mat4 lightSpaceMatrix = u_LightSpaceMatricesPoint[invocationID * 6 + i].mat;
		for(int k=0; k<3; k++)
		{
			gl_Position = lightSpaceMatrix * gl_in[k].gl_Position;
			EmitVertex();
		}
		EndPrimitive();
	}
	
}

#type fragment
#version 450 core

void main()
{
	// pass through shader no need to do anything
}