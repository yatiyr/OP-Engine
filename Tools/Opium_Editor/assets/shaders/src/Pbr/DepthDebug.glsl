#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 2) in vec2 a_TexCoords;

struct VS_OUT
{
	vec2 TexCoords;
};

layout(std140, binding = 1)  uniform Transform
{
	mat4 u_Model;
};

layout (location = 0) out VS_OUT vs_out;

void main()
{
	vs_out.TexCoords = a_TexCoords;
	gl_Position = u_Model * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

#define MAX_DIR_LIGHTS 4
#define MAX_SPOT_LIGHTS 4
#define MAX_CASCADE_SIZE 10

// Spot Light Data
struct SpotLight
{
	float Cutoff;
	float OuterCutoff;
	float NearDist;
	float FarDist;
	float Bias;
	float Kq;
	float Kl;
	vec3 LightDir;
	vec3 Color;
	vec3 Position;
};

layout(std140, binding = 4) uniform SpotLightData
{
	int u_SpotLightSize;
	SpotLight u_SpotLights[MAX_SPOT_LIGHTS];
};

layout(location = 0) out vec4 FragColor;

struct VS_OUT
{
	vec2 TexCoords;
};

layout (binding = 1) uniform sampler2DArray u_ShadowMapDirSpot;

layout (location = 0) in VS_OUT fs_in;

float PerspectiveProjDepthLinearize(float depth, float near, float far)
{
	float z = depth * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near - z * (far - near));

}
void main()
{
    float val = texture(u_ShadowMapDirSpot, vec3(fs_in.TexCoords, 40)).x;
	float far = u_SpotLights[0].FarDist;
	float near = u_SpotLights[0].NearDist;
	//float depthValue = texture(u_DepthMap, fs_in.TexCoords).r;
	// FragColor = vec4(vec3(LinearizeDepth(depthValue) / u_FarPlane), 1.0); // perspective
	FragColor = vec4(vec3(val), 1.0);
}