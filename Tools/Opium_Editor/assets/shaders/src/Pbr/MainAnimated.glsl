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

	vec4 totalPosition = vec4(0.0);
	int counter = 0;

    mat4 BoneTransform = u_BoneMatrices[a_BoneIDs[0]].mat * a_BoneWeights[0];
		BoneTransform += u_BoneMatrices[a_BoneIDs[1]].mat * a_BoneWeights[1];
		BoneTransform += u_BoneMatrices[a_BoneIDs[2]].mat * a_BoneWeights[2];
		BoneTransform += u_BoneMatrices[a_BoneIDs[3]].mat * a_BoneWeights[3];

	/*for(int i=0; i<MAX_BONE_INFLUENCE; i++)
	{
		if(a_BoneIDs[i] == -1)
		{
			counter++;
			continue;
		}
		if(a_BoneIDs[i] >= MAX_BONES)
		{
			totalPosition = vec4(a_Position, 1.0f);
			break;
		}

		BoneTransform += u_BoneMatrices[a_BoneIDs[i]].mat * a_BoneWeights[i];
		vec4 localPosition = u_BoneMatrices[a_BoneIDs[i]].mat * vec4(a_Position, 1.0f);
		totalPosition += localPosition * a_BoneWeights[i];
	} */
	/*if(a_BoneIDs[0] == -1 && a_BoneIDs[1] == -1 && a_BoneIDs[2] == -1 && a_BoneIDs[3] == -1)
		BoneTransform = mat4(1.0);*/

	totalPosition = BoneTransform * vec4(a_Position, 1.0);

	mat3 normalMatrix = transpose(inverse(mat3(BoneTransform)));
	vec3 T = normalize(normalMatrix * a_Tangent);
	vec3 B = normalize(normalMatrix * a_Bitangent);
	vec3 N = normalize(normalMatrix * a_Normal);
	vs_out.TBN = mat3(T, B, N);
	

	vs_out.FragPos = vec3(u_Model * totalPosition);
	vs_out.FragPosViewSpace = u_View * vec4(vs_out.FragPos, 1.0);
	vs_out.Normal  = transpose(inverse(mat3(u_Model))) * N;
	vs_out.TexCoords = a_TexCoords;

	gl_Position =  u_ViewProjection * u_Model * totalPosition;
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

// ------------- GLOBAL VARIABLES ------------- //
#include GlobalVariables.glsl
// -------------- UNIFORM BUFFERS ------------- //
#include UniformBuffers.glsl
// ----------------- UTILS -------------------- //
#include Utils.glsl
// ------------- SHADOW FUNCS ----------------- //
#include MainShadowFunctions.glsl

void main()
{
	vec3 fragPos = fs_in.FragPos;
	vec3 color   = u_Color;//texture(u_DiffuseTexture, fs_in.TexCoords).rgb;
	vec3 normal  = normalize(fs_in.Normal);

	vec3 ambient  = vec3(0.1);
	vec3 diffuse  = vec3(0.0);
	vec3 specular = vec3(0.0);
	vec3 result   = vec3(0.0);

	float shadow = 0.0;



	// Iterate directional lights
	for(int i=0; i<u_DirLightSize; i++)
	{
		
		int cascadeSize = u_DirLights[i].CascadeSize;
		vec3 lightColor = u_DirLights[i].Color;
		vec3 lightDir = -u_DirLights[i].LightDir;

		shadow     = ShadowCalculationDir(fragPos, lightDir, normal, cascadeSize,i);

		// ambient
		// ambient += 0.1 * lightColor;

		// diffuse
		float diff    = max(dot(lightDir, normal), 0.0);
		diffuse  += diff * lightColor * (1 - shadow);

		// specular
		vec3 viewDir    = normalize(u_ViewPos - fs_in.FragPos);
		float spec      = 0.1;
		vec3 halfwayDir = normalize(lightDir + viewDir);
		spec            = pow(max(dot(normal, halfwayDir), 0.0), 64);
		specular   += spec * lightColor * (1 - shadow);
	}

	for(int i=0; i<u_SpotLightSize; i++)
	{		
		SpotLight sL = u_SpotLights[i];

		vec3 lightColor = sL.Color;
		vec3 lightPos = sL.Position;
		vec3 lightDir = normalize(sL.LightDir);

		float cutoff = sL.Cutoff;
		float outerCutoff = sL.OuterCutoff;
		float epsilon = sL.Cutoff - sL.OuterCutoff;

		float dist = length(lightPos - fragPos);
		float Kq = u_SpotLights[i].Kq;
		float Kl = u_SpotLights[i].Kl;

		vec3 fragToLight = normalize(lightPos - fragPos);

		// Calculate attenuation
		
		float attenuation = 1 / (1 + Kq * dist * dist + Kl * dist);


		float theta = dot(fragToLight, -lightDir);
		float intensity = clamp((theta - outerCutoff) / epsilon, 0.0, 1.0);


		shadow = ShadowCalculationSpot(fragPos, lightDir, normal, i);

		float diff    = max(dot(fragToLight, normal), 0.0);
		diffuse  += diff * intensity * lightColor * (1 - shadow) * attenuation;

		// specular
		vec3 viewDir    = normalize(u_ViewPos - fs_in.FragPos);
		float spec      = 0.1;
		vec3 halfwayDir = normalize(-lightDir + viewDir);
		spec            = pow(max(dot(normal, halfwayDir), 0.0), 64);
		specular   += spec * lightColor * intensity * (1 - shadow) * attenuation;

	}

	for(int i=0; i<u_PointLightSize; i++)
	{
		PointLight pL = u_PointLights[i];

		vec3 lightColor = pL.Color;
		vec3 lightPos = pL.Position;
		vec3 lightDir = normalize(lightPos - fragPos);
		float dist = length(lightPos - fragPos);
		float Kq = u_PointLights[i].Kq;
		float Kl = u_PointLights[i].Kl;

		// Calculate attenuation
		float attenuation = 1 / (1 + Kq * dist * dist + Kl * dist);

		shadow = ShadowCalculationPoint(fragPos, u_ViewPos, u_ShadowMapPoint, i);

		// diffuse
		float diff = max(dot(lightDir, normal), 0.0);
		diffuse += diff * lightColor * (1 - shadow) * attenuation;

		// specular
		vec3 viewDir = normalize(u_ViewPos - fs_in.FragPos);
		float spec = 0.1;
		vec3 halfwayDir = normalize(lightDir + viewDir);
		spec = pow(max(dot(normal, halfwayDir), 0.0), 64);
		specular += spec * lightColor * (1 - shadow) * attenuation;

	}

	vec3 lighting    =  (ambient + (diffuse + specular)) * color ;

	FragColor = vec4(lighting, 1.0);
}