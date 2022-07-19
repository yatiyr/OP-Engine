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

layout(std140, binding = 5) uniform LightSpaceMatricesDSData
{
	LightSpaceMat u_LightSpaceMatricesDirSpot[MAX_DIR_LIGHTS * MAX_CASCADE_SIZE + MAX_SPOT_LIGHTS];
};