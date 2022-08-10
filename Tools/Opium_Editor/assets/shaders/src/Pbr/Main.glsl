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
	vs_out.Normal  = modelMatrixN * a_Normal;
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
layout (binding = 2) uniform samplerCube u_IrradianceMap;

layout (location = 0) uniform float u_Roughness;
layout (location = 1) uniform float u_Metalness;
layout (location = 2) uniform vec3 u_Albedo;
layout (location = 3) uniform float u_TilingFactor;
layout (location = 4) uniform float u_HeightFactor;
layout (location = 5) uniform int u_ClipBorder;

layout (binding = 3) uniform sampler2D u_albedoMap;
layout (binding = 4) uniform sampler2D u_RoughnessMap;
layout (binding = 5) uniform sampler2D u_MetalnessMap;
layout (binding = 6) uniform sampler2D u_AoMap;
layout (binding = 7) uniform sampler2D u_NormalMap;
layout (binding = 8) uniform sampler2D u_HeightMap;
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
#include ParallaxMapping.glsl
// -------------------------------------------- //

void main()
{

	vec2 texCoords = fs_in.TexCoords;
	vec3 fragPos = fs_in.FragPos;
	
	vec3 viewDir = normalize(u_ViewPos - fs_in.FragPos);

	float parallaxHeight;
	texCoords = ParallaxMapping(u_HeightMap, vec3(0.0, 0.0, 1.0), fs_in.TexCoords, normalize(transpose(fs_in.TBN) * viewDir), parallaxHeight, u_TilingFactor, u_HeightFactor);
	if((texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0) && u_ClipBorder > 0)
	    discard;

	vec3 color      = u_Albedo * pow(texture(u_albedoMap, texCoords * u_TilingFactor).rgb, vec3(2.2));
	float roughness = u_Roughness * texture(u_RoughnessMap, texCoords * u_TilingFactor).r;
	float metalness = u_Metalness * texture(u_MetalnessMap, texCoords * u_TilingFactor).r;
	float ao = texture(u_AoMap, texCoords * u_TilingFactor).r;

	vec3 normal = texture(u_NormalMap, texCoords * u_TilingFactor).rgb;
	normal = normalize(normal * 2.0 - 1.0);
	normal = normalize(fs_in.TBN * normal);

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
		float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.001;
		vec3 specular = numerator / denominator;

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metalness;

		float backFacingFactor = dot(normalize(fs_in.Normal), lightDir) > 0 ? 1.0 : 0.0;

		float NdotL = max(dot(normal, lightDir), 0.0);

		float layerHeight;
		float shadowMultiplier = parallaxShadowMultiplier(u_HeightMap, vec3(0.0, 0.0, 1.0), normalize(transpose(fs_in.TBN) * lightDir), texCoords, parallaxHeight - 0.01, layerHeight, u_TilingFactor, u_HeightFactor);

		Lo += (kD * color / PI + specular) * radiance * NdotL * (1 - shadow) * pow(shadowMultiplier, 16) * backFacingFactor;
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
		float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, -lightDir), 0.0) + 0.001;
		vec3 specular = numerator / denominator;

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metalness;

		float backFacingFactor = dot(normalize(fs_in.Normal), -lightDir) > 0 ? 1.0 : 0.0;

		float NdotL = max(dot(normal, -lightDir), 0.0);

		float layerHeight;
		float shadowMultiplier = parallaxShadowMultiplier(u_HeightMap, vec3(0.0, 0.0, 1.0), -normalize(transpose(fs_in.TBN) * lightDir), texCoords, parallaxHeight - 0.01, layerHeight, u_TilingFactor,  u_HeightFactor);
		Lo += (kD * color / PI + specular) * radiance * NdotL * (1 - shadow) * intensity * pow(shadowMultiplier, 16) * backFacingFactor;

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
		float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.001;
		vec3 specular = numerator / denominator;

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metalness;

		float backFacingFactor = dot(normalize(fs_in.Normal), lightDir) > 0 ? 1.0 : 0.0;
		float NdotL = max(dot(normal, lightDir), 0.0);

		float layerHeight;
		float shadowMultiplier = parallaxShadowMultiplier(u_HeightMap, vec3(0.0, 0.0, 1.0), normalize(transpose(fs_in.TBN) * lightDir), texCoords, parallaxHeight - 0.01, layerHeight, u_TilingFactor,  u_HeightFactor);
		Lo += (kD * color / PI + specular) * radiance * NdotL * (1 - shadow) * pow(shadowMultiplier, 16) * backFacingFactor;

	}

	vec3 kS = FresnelSchlick(max(dot(normal, viewDir), 0.0), F0); 
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - metalness;
	vec3 irradiance = texture(u_IrradianceMap, normal).rgb;
	vec3 diffuse    = irradiance * color;
	vec3 ambient    = (kD * diffuse) * ao; 
	//vec3 ambient = vec3(0.03) * color * ao;

	vec3 lighting    =  (ambient + Lo);

	FragColor = vec4(lighting, 1.0);
}