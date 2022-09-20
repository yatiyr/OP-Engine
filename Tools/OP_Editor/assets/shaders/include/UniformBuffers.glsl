layout(std140, binding = 0) uniform CubemapCaptureView
{
	mat4 u_CaptureViews[6];
};

layout(std140, binding = 1) uniform Camera
{
	mat4 u_ViewProjection;
	mat4 u_View;
	mat4 u_Projection;
	vec3 u_ViewPos;
	float u_Near;
	float u_Far;
};

layout(std140, binding = 2)  uniform Transform
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

layout(std140, binding = 3) uniform ShadowMapSettings
{
	float u_ShadowMapResX;
	float u_ShadowMapResY;
	float u_PointLightSMResX;
	float u_PointLightSMResY;
	vec2 u_BlurScale;
};

layout(std140, binding = 4) uniform DirLightData
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

layout(std140, binding = 5) uniform SpotLightData
{
	int u_SpotLightSize;
	SpotLight u_SpotLights[MAX_SPOT_LIGHTS];
};

struct PointLight
{
	float NearDist;
	float FarDist;
	float Kq;
	float Kl;
	vec3 Color;
	vec3 Position;
};

layout(std140, binding = 6) uniform PointLightData
{
	int u_PointLightSize;
	PointLight u_PointLights[MAX_POINT_LIGHTS];
};

layout(std140, binding = 7) uniform LightSpaceMatricesDSData
{
	LightSpaceMat u_LightSpaceMatricesDirSpot[MAX_DIR_LIGHTS * MAX_CASCADE_SIZE + MAX_SPOT_LIGHTS];
};

layout(std140, binding = 8) uniform LightSpaceMatricesPointData
{
	LightSpaceMat u_LightSpaceMatricesPoint[MAX_POINT_LIGHTS * 6];
};

struct CascadePlane
{
	float dist;
	float _align1;
	float _align2;
	float _align3;
};

layout(std140, binding = 9) uniform CascadePlaneDistancesData
{
	CascadePlane u_CascadePlanes[(MAX_CASCADE_SIZE - 1) * MAX_DIR_LIGHTS];
};

layout(std140, binding = 10) uniform MaterialData
{
	vec3 u_Color;
};

layout(std140, binding = 11) uniform ToneMappingData
{
	float u_Exposure;
	bool u_Hdr;
};


struct BoneMat
{
	mat4 mat;
};

layout(std140, binding = 12) uniform BoneMatricesData
{
	BoneMat u_BoneMatrices[MAX_BONES];
};