#type vertex
#version 450 core

// ------------------ DEFINES ----------------- //
#include Defines.glsl
// -------------------------------------------- //

layout(location = 0) in vec3 a_Position;

// -------------- UNIFORM BUFFERS ------------- //
#include UniformBuffers.glsl
// -------------------------------------------- //

void main()
{
	gl_Position = vec4(a_Position, 1.0);
}

#type geometry
#version 450 core


// ------------------ DEFINES ----------------- //
#include Defines.glsl
// -------------------------------------------- //

layout (triangles, invocations = 6) in;
layout (triangle_strip, max_vertices = 3) out;

// ---------------- UNIFORM BUFFERS ------------- //
#include UniformBuffers.glsl
// ---------------------------------------------- //

struct GS_OUT
{
	vec3 WorldPos;
};

layout (location = 1) out GS_OUT gs_out;

layout (location = 0) uniform mat4 u_CaptureProjection;

void main()
{
	int invocationID = gl_InvocationID;

	// We are rendering to 6 faces of the cubemap
	gl_Layer = invocationID;
	for(int k=0; k<3; k++)
	{
		gs_out.WorldPos = gl_in[k].gl_Position.xyz;
		gl_Position = u_CaptureProjection * u_CaptureViews[gl_Layer] * gl_in[k].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}


#type fragment
#version 450 core

// ------------------ DEFINES ----------------- //
#include Defines.glsl
// -------------------------------------------- //

layout(location = 0) out vec4 FragColor;

struct GS_OUT
{
	vec3 WorldPos;
};

layout (location = 1) in GS_OUT fs_in;

// ---------------- UNIFORM BUFFERS ------------- //
#include UniformBuffers.glsl
// ---------------------------------------------- //

// ------------------- UTILS -------------------- //
#include Utils.glsl
// ---------------------------------------------- //

// -------------- GLOBAL VARIABLES -------------- //
#include GlobalVariables.glsl
// ---------------------------------------------- //

layout (binding = 0) uniform samplerCube u_Cubemap;

const vec2 invAtan = vec2(0.1591, 0.3183);


void main()
{
	vec3 N = normalize(fs_in.WorldPos);
	vec3 irradiance = vec3(0.0);

	// convolution
	vec3 up    = vec3(0.0, 1.0, 0.0);
	vec3 right = normalize(cross(up, N));
	up         = normalize(cross(N, right));

	float sampleDelta = 0.025;
	float nrSamples = 0.0; 
	for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
	{
		for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
		{
			// spherical to cartesian (in tangent space)
			vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
			// tangent space to world
			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N; 

			irradiance += textureLod(u_Cubemap, sampleVec, 0.0).rgb * cos(theta) * sin(theta);
			nrSamples++;
		}
	}
	irradiance = PI * irradiance * (1.0 / float(nrSamples));

	FragColor = vec4(irradiance, 1.0);
}