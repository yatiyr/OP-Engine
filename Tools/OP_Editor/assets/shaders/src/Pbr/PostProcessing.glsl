#type vertex
#version 450 core

// ------------------ DEFINES ----------------- //
#include Defines.glsl
// -------------------------------------------- //

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

#type fragment
#version 450 core

layout(location = 0) out vec4 FragColor;

struct VS_OUT
{
	vec2 TexCoords;
};

layout (location = 0) in VS_OUT fs_in;

layout (binding = 0) uniform sampler2D u_Image;

// ------------------ DEFINES ----------------- //
#include Defines.glsl
// -------------------------------------------- //

// -------------- UNIFORM BUFFERS ------------- //
#include UniformBuffers.glsl
// -------------------------------------------- //

const highp float NOISE_GRANULARITY = 0.5/255.0;

highp float random(highp vec2 coords) {
   return fract(sin(dot(coords.xy, vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	const float gamma = 2.2;
	vec3 hdrColor = texture(u_Image, fs_in.TexCoords).rgb;

	// exposure tone mapping
	vec3 mapped = vec3(1.0) - exp(-hdrColor * u_Exposure);

	// reinhard tone mapping
	//vec3 mapped = hdrColor / (hdrColor + vec3(1.0));

	// gamma correction
	if(u_Hdr)
		mapped = pow(mapped, vec3(1.0 / ( gamma)));

	mapped += mix(-NOISE_GRANULARITY, NOISE_GRANULARITY, random(fs_in.TexCoords));

	FragColor = vec4(mapped, 1.0);
}