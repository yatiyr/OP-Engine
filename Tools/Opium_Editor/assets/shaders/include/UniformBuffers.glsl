layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
	mat4 u_View;
	vec3 u_ViewPos;
};

layout(std140, binding = 1)  uniform Transform
{
	mat4 u_Model;
};

struct LightSpaceMat
{
	mat4 mat;
};

// Directional Light Data
struct DirLight
{ 
	int CascadeSize;
	float FrustaDistFactor;
	vec3 LightDir;
	vec3 Color;
};

layout(std140, binding = 2) uniform ShadowMapSettings
{
	float u_ShadowMapResX;
	float u_ShadowMapResY;
	vec2 u_BlurScale;
};

layout(std140, binding = 3) uniform DirLightData
{
	int u_DirLightSize;
	DirLight u_DirLights[MAX_DIR_LIGHTS];
};

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

layout(std140, binding = 5) uniform LightSpaceMatricesDSData
{
	LightSpaceMat u_LightSpaceMatricesDirSpot[MAX_DIR_LIGHTS * MAX_CASCADE_SIZE + MAX_SPOT_LIGHTS];
};

struct CascadePlane
{
	float dist;
	float _align1;
	float _align2;
	float _align3;
};

layout(std140, binding = 6) uniform CascadePlaneDistancesData
{
	CascadePlane u_CascadePlanes[(MAX_CASCADE_SIZE - 1) * MAX_DIR_LIGHTS];
};

layout(std140, binding = 7) uniform MaterialData
{
	vec3 u_Color;
};