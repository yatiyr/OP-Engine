#type vertex
#version 450 core

// ------------------ DEFINES ----------------- //
#include Defines.glsl
// -------------------------------------------- //

layout(location = 0) in vec3 a_Position;

struct VS_OUT
{
	vec3 NearPoint;
	vec3 FarPoint;
};

// -------------- UNIFORM BUFFERS ----------- //
#include UniformBuffers.glsl
// ------------------------------------------ //

layout (location = 0) out VS_OUT vs_out;

vec3 UnprojectPoint(float x, float y, float z, mat4 view, mat4 projection)
{
	mat4 viewInv = inverse(view);
	mat4 projInv = inverse(projection);

	vec4 unprojectedPoint = viewInv * projInv * vec4(x, y, z, 1.0);

	return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main()
{
	vec3 point = a_Position;
	vs_out.NearPoint = UnprojectPoint(point.x, point.y, 0.0, u_View, u_Projection).xyz;
	vs_out.FarPoint  = UnprojectPoint(point.x, point.y, 1.0, u_View, u_Projection).xyz;
	gl_Position = vec4(point, 1.0);
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
	vec3 NearPoint;
	vec3 FarPoint;
};

layout (location = 0) in VS_OUT fs_in;

// -------------------- UTILS ----------------- //
#include Utils.glsl
// -------------------------------------------- //

vec4 grid(vec3 fragPos3D, float scale, bool drawAxis) {
    vec2 coord = fragPos3D.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));
    // z axis
    if(fragPos3D.x > -0.1 * minimumx && fragPos3D.x < 0.1 * minimumx)
    {
        color.y = 0.5;
        color.z = 1.0;
    }
        
    // x axis
    if(fragPos3D.z > -0.1 * minimumz && fragPos3D.z < 0.1 * minimumz)
    {
        color.x = 1.0;
        color.z = 0.3;
    }
   
    return color;
}

float computeDepth(vec3 pos) {
    vec4 clip_space_pos = u_Projection * u_View * vec4(pos.xyz, 1.0);
    return 0.5 + 0.5 * (clip_space_pos.z / clip_space_pos.w);
}

float computeLinearDepth(vec3 pos)
{
    vec4 clip_space_pos = u_Projection * u_View * vec4(pos.xyz, 1.0);
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0;
    float linearDepth = ( 2.0 * 0.1 * 10.0) / (10.0 + 0.1 - clip_space_depth * (10.0 - 0.1));

    return linearDepth / 10.0;
}

void main()
{
    float t = -fs_in.NearPoint.y / (fs_in.FarPoint.y - fs_in.NearPoint.y);

	vec3 FragPos3D = fs_in.NearPoint + t * (fs_in.FarPoint - fs_in.NearPoint);
	gl_FragDepth = computeDepth(FragPos3D);

    float linearDepth = computeLinearDepth(FragPos3D);
    float fading = max(0, (0.5 - linearDepth));

	FragColor = (grid(FragPos3D, 10, true) + grid(FragPos3D, 1, true)) * float(t > 0);//vec4(1.0, 0.0, 0.0, 1.0 * float(t > 0));
    FragColor.w *= fading;
    FragColor.w *= 0.7;
}