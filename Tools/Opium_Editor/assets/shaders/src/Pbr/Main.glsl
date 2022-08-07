#type vertex
#version 450 core

// ------------------ DEFINES ----------------- //
#include Defines.glsl
// -------------------------------------------- //

layout (location = 0) in vec3  a_Position;
layout (location = 1) in vec3  a_Normal;
layout (location = 2) in vec2  a_TexCoords;
layout (location = 3) in vec3  a_Tangent;
layout (location = 4) in vec3  a_Bitangent;
layout (location = 5) in ivec4 a_BoneIDs;
layout (location = 6) in vec4  a_BoneWeights;


struct VS_OUT
{
	vec3 FragPos;
	vec4 FragPosViewSpace;
	vec3 Normal;
	vec2 TexCoords;
	mat3 TBN;
};

layout (location = 0) out VS_OUT vs_out;


// -------------- UNIFORM BUFFERS ------------- //
#include UniformBuffers.glsl
// -------------------------------------------- //


void main()
{

	mat3 modelMatrixN = transpose(inverse(mat3(u_Model)));

	vec3 T = normalize(modelMatrixN * a_Tangent);
	vec3 B = normalize(modelMatrixN * a_Bitangent);
	vec3 N = normalize(modelMatrixN * a_Normal);
	vs_out.TBN = mat3(T, B, N);
	

	vs_out.FragPos = vec3(u_Model * vec4(a_Position, 1.0));
	vs_out.FragPosViewSpace = u_View * vec4(vs_out.FragPos, 1.0);
	vs_out.Normal  = transpose(inverse(mat3(u_Model))) * N;
	vs_out.TexCoords = a_TexCoords;

	gl_Position =  u_ViewProjection * u_Model * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

// ------------------ DEFINES ----------------- //
#include Defines.glsl
// -------------------------------------------- //


layout(location = 0) out vec4 FragColor;

struct VS_OUT
{
	vec3 FragPos;
	vec4 FragPosViewSpace;
	vec3 Normal;
	vec2 TexCoords;
	mat3 TBN;
};

layout (location = 0) in VS_OUT fs_in;

layout (binding = 0) uniform sampler2DArray u_ShadowMapDirSpot;
layout (binding = 1) uniform samplerCubeArray u_ShadowMapPoint;

layout (location = 0) uniform float u_Roughness;
layout (location = 1) uniform float u_Metalness;
layout (location = 2) uniform vec3 u_Albedo;


layout (binding = 2) uniform sampler2D u_albedoMap;
layout (binding = 3) uniform sampler2D u_RoughnessMap;
layout (binding = 4) uniform sampler2D u_MetalnessMap;
layout (binding = 5) uniform sampler2D u_AoMap;
layout (binding = 6) uniform sampler2D u_NormalMap;
layout (binding = 7) uniform sampler2D u_HeightMap;

// ------------- GLOBAL VARIABLES ------------- //
#include GlobalVariables.glsl
// -------------- UNIFORM BUFFERS ------------- //
#include UniformBuffers.glsl
// ----------------- UTILS -------------------- //
#include Utils.glsl
// ------------- SHADOW FUNCS ----------------- //
#include MainShadowFunctions.glsl
// -------------- PBR FUNCS ------------------- //
#include PbrFunctions.glsl
// -------------------------------------------- //

void main()
{
	vec3 fragPos = fs_in.FragPos;

	vec3 color      = u_Albedo * pow(texture(u_albedoMap, fs_in.TexCoords).rgb, vec3(2.2));
	float roughness = u_Roughness * texture(u_RoughnessMap, fs_in.TexCoords).r;
	float metalness = u_Metalness * texture(u_MetalnessMap, fs_in.TexCoords).r;
	float ao = texture(u_AoMap, fs_in.TexCoords).r;

	vec3 normal = texture(u_NormalMap, fs_in.TexCoords).rgb;
	normal = normalize(normal * 2.0 - 1.0);
	normal = normalize(fs_in.TBN * normal);

	vec3 viewDir = normalize(u_ViewPos - fs_in.FragPos);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, color, metalness);


	vec3 Lo = vec3(0.0);
	float shadow = 0.0;

	// Iterate directional lights
	for(int i=0; i<u_DirLightSize; i++)
	{
		
		int cascadeSize = u_DirLights[i].CascadeSize;
		vec3 lightDir   = -u_DirLights[i].LightDir;
		shadow     = ShadowCalculationDir(fragPos, lightDir, normal, cascadeSize,i);


		vec3 radiance = u_DirLights[i].Color;
		vec3 halfwayDir = normalize(viewDir + lightDir);
		

		float NDF = DistributionGGX(normal, halfwayDir, roughness);
		float G   = GeometrySmith(normal, viewDir, lightDir, roughness);
		vec3  F   = FresnelSchlick(max(dot(halfwayDir, viewDir), 0.0), F0);

		vec3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
		vec3 specular = numerator / denominator;

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metalness;

		float NdotL = max(dot(normal, lightDir), 0.0);

		Lo += (kD * color / PI + specular) * radiance * NdotL * (1 - shadow);
	}

	for(int i=0; i<u_SpotLightSize; i++)
	{		
		SpotLight sL = u_SpotLights[i];

		vec3 lightPos = sL.Position;
		vec3 lightDir = normalize(sL.LightDir);
		float dist = length(lightPos - fragPos);
		vec3 fragToLight = normalize(lightPos - fragPos);
		vec3 halfwayDir = normalize(viewDir - lightDir);

		float cutoff = sL.Cutoff;
		float outerCutoff = sL.OuterCutoff;
		float epsilon = sL.Cutoff - sL.OuterCutoff;

		float Kq = u_SpotLights[i].Kq;
		float Kl = u_SpotLights[i].Kl;


		// Calculate attenuation
		float attenuation = 1 / (dist * dist);
		vec3 radiance = sL.Color; // * attenuation;

		float theta = dot(fragToLight, -lightDir);
		float intensity = clamp((theta - outerCutoff) / epsilon, 0.0, 1.0);


		shadow = ShadowCalculationSpot(fragPos, lightDir, normal, i);

		float NDF = DistributionGGX(normal, halfwayDir, roughness);
		float G   = GeometrySmith(normal, viewDir, -lightDir, roughness);
		vec3  F   = FresnelSchlick(max(dot(halfwayDir, viewDir), 0.0), F0);

		vec3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, -lightDir), 0.0) + 0.0001;
		vec3 specular = numerator / denominator;

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metalness;

		float NdotL = max(dot(normal, -lightDir), 0.0);

		Lo += (kD * color / PI + specular) * radiance * NdotL * (1 - shadow) * intensity;

	}

	for(int i=0; i<u_PointLightSize; i++)
	{
		PointLight pL = u_PointLights[i];

		vec3 lightPos = pL.Position;
		vec3 lightDir = normalize(lightPos - fragPos);
		float dist = length(lightPos - fragPos);
		vec3 halfwayDir = normalize(viewDir + lightDir);

		// Calculate attenuation
		float attenuation = 1 / (dist * dist);
		vec3 radiance = pL.Color * attenuation;

		shadow = ShadowCalculationPoint(fragPos, u_ViewPos, u_ShadowMapPoint, i);

		float NDF = DistributionGGX(normal, halfwayDir, roughness);
		float G   = GeometrySmith(normal, viewDir, lightDir, roughness);
		vec3  F   = FresnelSchlick(max(dot(halfwayDir, viewDir), 0.0), F0);

		vec3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
		vec3 specular = numerator / denominator;

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metalness;

		float NdotL = max(dot(normal, lightDir), 0.0);

		Lo += (kD * color / PI + specular) * radiance * NdotL * (1 - shadow);

	}

	vec3 ambient = vec3(0.03) * color * ao;

	vec3 lighting    =  ambient + Lo;

	FragColor = vec4(lighting, 1.0);
}