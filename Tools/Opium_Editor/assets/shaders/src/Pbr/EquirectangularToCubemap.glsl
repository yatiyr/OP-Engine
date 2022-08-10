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

layout (binding = 0) uniform sampler2D u_EquirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v)
{
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	uv *= invAtan;
	uv += 0.5;
	return uv;
}

void main()
{
	vec2 uv = SampleSphericalMap(normalize(fs_in.WorldPos));
	vec3 color = texture(u_EquirectangularMap, uv).rgb;

	FragColor = vec4(color, 1.0);
}