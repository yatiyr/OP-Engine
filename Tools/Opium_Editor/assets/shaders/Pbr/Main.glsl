#type vertex
#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoords;
layout (location = 3) in vec3 a_Tangent;
layout (location = 4) in vec3 a_Bitangent;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
	vec3 view_Pos;
};

struct VS_OUT
{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
	vec3 Color;
};

layout (location = 0) out VS_OUT vs_out;

layout(std140, binding = 1) uniform Matrices
{
	mat4 u_Model;
	mat4 u_LightSpaceMatrix;
	vec3 u_Color;
};


void main()
{
	vs_out.FragPos = vec3(u_Model * vec4(a_Position, 1.0));
	vs_out.Normal  = transpose(inverse(mat3(u_Model))) * a_Normal;
	vs_out.TexCoords = a_TexCoords;
	vs_out.FragPosLightSpace = u_LightSpaceMatrix * vec4(vs_out.FragPos, 1.0);

	gl_Position =  u_ViewProjection * u_Model * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 FragColor;

struct VS_OUT
{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
};

layout (location = 0) in VS_OUT fs_in;

layout (binding = 0) uniform sampler2D u_DiffuseTexture;
layout (binding = 1) uniform sampler2D u_ShadowMap;

layout(std140, binding = 1) uniform Matrices
{
	mat4 u_Model;
	mat4 u_LightSpaceMatrix;
	vec3 u_Color;
};


layout(std140, binding = 2) uniform LightPos
{
	vec3 u_LightPos;
};


layout(std140, binding = 3) uniform ViewPos
{
	vec3 u_ViewPos;
};


vec2 poissonDisk[16] = vec2[]
(
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

float CalculateVisibility(vec4 fragPosLightSpace, vec3 lightDir, vec3 normal)
{
	float visibility = 1.0;

	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

	// transform to [0, 1] range
	projCoords = projCoords * 0.5 + 0.5;

	float cosTheta = clamp(dot(normal, lightDir), 0.0, 1.0);
	float bias = 0.0005 * tan(acos(cosTheta));
	clamp(bias, 0, 0.01);

	if(projCoords.z > 1.0)
		return 1.0;


	for(int i=0; i<16; i++)
	{
		if(texture(u_ShadowMap, projCoords.xy + poissonDisk[i]/1400.0).r < projCoords.z - bias)
			visibility -= 0.05;
	}


	return visibility;

}


float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir, vec3 normal)
{
	// perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

	// transform to [0, 1] range
	projCoords = projCoords * 0.5 + 0.5;

	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	float closestDepth = texture(u_ShadowMap, projCoords.xy).r;

	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;


	float cosTheta = clamp(dot(normal, lightDir), 0.0, 1.0);
	// check whether current frag pos is in shadow
	// float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);

	// check whether current frag pos is in shadow
	// float shadow = current depth - bias > closestDepth ? 1.0 : 0.0;
	// PCF
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x,y) * texelSize).r;
			shadow += currentDepth > pcfDepth ? 1.0 : 0.0;
		}
	}

	shadow /= 9.0;

	if(projCoords.z > 1.0)
		return 0.0;

	return shadow;

}

void main()
{
	vec3 color = texture(u_DiffuseTexture, fs_in.TexCoords).rgb;
	vec3 normal = normalize(fs_in.Normal);

	vec3 lightColor = vec3(0.3);

	// ambient
	vec3 ambient = 0.3 * lightColor;

	// diffuse
	vec3 lightDir = normalize(u_LightPos - fs_in.FragPos);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * lightColor;

	// specular
	vec3 viewDir = normalize(u_ViewPos - fs_in.FragPos);
	float spec = 0.0;
	vec3 halfwayDir = normalize(lightDir + viewDir);
	spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
	vec3 specular = spec * lightColor;

	// calculate shadow
	float visibility = CalculateVisibility(fs_in.FragPosLightSpace, lightDir, normal);

	float shadow = ShadowCalculation(fs_in.FragPosLightSpace, lightDir, normal);
	vec3 lighting = (ambient + (visibility) * (diffuse + specular)) * color;

	FragColor = vec4(u_Color,1.0);//vec4(lighting, 1.0);
}