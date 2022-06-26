#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;

struct VS_OUT
{
	vec2 TexCoords;
};

layout (location = 0) out VS_OUT vs_out;

void main()
{
	vs_out.TexCoords = a_TexCoords;
	gl_Position = vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 FragColor;

struct VS_OUT
{
	vec2 TexCoords;
};

layout (location = 0) in VS_OUT fs_in;

layout (binding = 0) uniform sampler2D u_DepthMap;

layout(std140, binding = 1) uniform FarNearPlane
{
	float u_NearPlane;
	float u_FarPlane;
};

// required when using a perspective projection matrix
float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0; // Back to NDC
	return (2.0 * u_NearPlane * u_FarPlane) / (u_FarPlane + u_NearPlane - z * (u_FarPlane - u_NearPlane));
}

void main()
{
	float depthValue = texture(u_DepthMap, fs_in.TexCoords).r;
	// FragColor = vec4(vec3(LinearizeDepth(depthValue) / u_FarPlane), 1.0); // perspective
	FragColor = vec4(vec3(depthValue), 1.0); // orthographic
}