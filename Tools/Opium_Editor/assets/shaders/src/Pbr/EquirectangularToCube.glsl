#type vertex
#version 450 core

// ------------------ DEFINES ----------------- //
#include Defines.glsl
// -------------------------------------------- //

layout(location = 0) in vec3 a_Position;

struct VS_OUT
{
	vec3 LocalPosition;
};

// -------------- UNIFORM BUFFERS ----------- //
#include UniformBuffers.glsl
// ------------------------------------------ //

layout (location = 0) out VS_OUT vs_out;

void main()
{
	vs_out.LocalPosition = a_Position;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

// ------------------ DEFINES ----------------- //
#include Defines.glsl
// -------------------------------------------- //

// -------------- UNIFORM BUFFERS ----------- //
#include UniformBuffers.glsl
// ------------------------------------------ //

layout(location = 0) out vec4 FragColor;

struct VS_OUT
{
	vec3 LocalPosition;
};

layout (binding = 1) uniform sampler2D u_EquirectangularMap;

layout (location = 0) in VS_OUT fs_in;

// -------------------- UTILS ----------------- //
#include Utils.glsl
// -------------------------------------------- //

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
	vec2 uv = SampleSphericalMap(normalize(fs_in.LocalPosition));
	vec3 color = texture(u_EquirectangularMap, uv).rgb;

	FragColor = vec4(color, 1.0);
}