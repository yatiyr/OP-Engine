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
layout (binding = 1) uniform sampler2D u_BayerDithering;

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

	vec3 firstColor = texture(u_EquirectangularMap, uv).rgb;
	vec3 color = firstColor;

	vec4 intermediateCol = vec4(color, 1.0);
	intermediateCol += vec4(texture(u_BayerDithering, gl_FragCoord.xy / 8.0).r / 128.0 - (1.0 / 128.0));

	color = vec3(intermediateCol);

	// luma based reinhard tone mapping
	float luma = dot(color, vec3(0.2126, 0.7152, 0.0722));
	float toneMappedLuma = luma / (1.0 + luma);
	color *= toneMappedLuma / luma;
    
	
	if(isnan(color.x))
		color.x = 0.999;

	if(isnan(color.y))
		color.y = 0.999;

	if(isnan(color.z))
		color.z = 0.999;

	FragColor = vec4(color, 1.0);
}