
float SampleVarianceShadowMap(sampler2DArray shadowMap, vec3 coords, float compare)
{

	vec2 moments = texture(shadowMap, coords.xyz).xy;

	float p = step(compare, moments.x);
	float variance = max(moments.y - moments.x * moments.x, 0.0000002);

	float d = compare - moments.x;
	float pMax = linstep(0.2, 1.0,variance / (variance + d*d));

	return min(max(p, pMax), 1.0);
}


float ShadowCalculationSpot(vec3 fragPosWorldSpace, vec3 lightDir, vec3 normal, int spotLightIndex)
{

	vec4 fragPosLightSpace = u_LightSpaceMatricesDirSpot[(MAX_CASCADE_SIZE * MAX_DIR_LIGHTS)  + spotLightIndex].mat *
		vec4(fragPosWorldSpace, 1.0);

	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;

	float fragPosDist = fragPosLightSpace.z;

	float currentDistance = length(fragPosWorldSpace - u_SpotLights[spotLightIndex].Position);
	float compareDistance = texture(u_ShadowMapDirSpot, vec3(projCoords.xy,  (MAX_CASCADE_SIZE * MAX_DIR_LIGHTS)  + spotLightIndex)).r * u_SpotLights[spotLightIndex].FarDist;

	float currentDepth =  projCoords.z * u_SpotLights[spotLightIndex].FarDist;

	if(currentDepth >= u_SpotLights[spotLightIndex].FarDist)
		return 1.0;

	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.05);
	float shadow = 0.0;
	//shadow = (compareDistance + 0.15) < fragPosDist ? 0.0 : 1.0;

	
	// PCF
	vec2 texelSize = 1.0 / vec2(u_ShadowMapResX, u_ShadowMapResY);
	for (int x=-5; x<=5; x++)
	{
		for (int y=-5; y<=5; y++)
		{
			float pcfDepth = texture(u_ShadowMapDirSpot, vec3(projCoords.xy + vec2(x,y) * texelSize,  (MAX_CASCADE_SIZE * MAX_DIR_LIGHTS)  + spotLightIndex)).r * u_SpotLights[spotLightIndex].FarDist;
			shadow += (pcfDepth + bias) < fragPosDist ? 0.0 : 1.0;
		}
	}

	shadow /= 9.0; 

	return 1 - shadow;

}

float ShadowCalculationDir(vec3 fragPosWorldSpace, vec3 lightDir, vec3 normal,  int cascadeSize, int dirLightIndex)
{
	// get cascade plane
	vec4 fragPosViewSpace = fs_in.FragPosViewSpace;

	float depthVal = abs(fragPosViewSpace.z);

	int layer = -1;
	for(int i=0; i<cascadeSize-1; i++)
	{
		if (depthVal < u_CascadePlanes[(MAX_CASCADE_SIZE - 1) * dirLightIndex + i].dist)
		{
			layer = i;
			break;
		}
	}

	if(layer == -1)
	{
		layer = cascadeSize - 1;
	}


	vec4 fragPosLightSpace = u_LightSpaceMatricesDirSpot[(MAX_CASCADE_SIZE) * dirLightIndex + layer].mat *
	                         vec4(fragPosWorldSpace, 1.0);
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

	projCoords = projCoords * 0.5 + 0.5;

	// current depth from light's perspective
	float currentDepth = projCoords.z;

	if (currentDepth > 1.0)
	{
		return 0.0;
	}

	float bias = max(0.0025 * (1.0 - dot(normal, lightDir)), 0.0025);
	float shadow = 0.0;
	/*//shadow = (compareDistance + 0.15) < fragPosDist ? 0.0 : 1.0;

	
	// PCF
	vec2 texelSize = 1.0 / vec2(u_ShadowMapResX, u_ShadowMapResY);
	for (int x=-5; x<=5; x++)
	{
		for (int y=-5; y<=5; y++)
		{
			float pcfDepth = texture(u_ShadowMapDirSpot, vec3(projCoords.xy + vec2(x,y) * texelSize,  (MAX_CASCADE_SIZE * dirLightIndex)  + layer)).r;
			shadow += (pcfDepth + bias) < currentDepth ? 0.0 : 1.0;
		}
	}

	shadow /= 9.0; 

	return 1 - shadow; */

	shadow = SampleVarianceShadowMap(u_ShadowMapDirSpot, vec3(projCoords.x, projCoords.y, (MAX_CASCADE_SIZE) * dirLightIndex + layer ), currentDepth);

	return 1 - shadow;
}


float ShadowCalculationPoint(vec3 fragPosWorldSpace, vec3 viewPos, samplerCubeArray shadowMapPoint, int pointLightIndex)
{
	vec3 fragToLight = fragPosWorldSpace - u_PointLights[pointLightIndex].Position;

	float currentDepth = length(fragToLight);

	float shadow = 0.0;
	float bias = 0.05;
	int samples = 20;
	float viewDistance = length(viewPos - fragPosWorldSpace);
	float diskRadius = (1.0 + (viewDistance / u_PointLights[pointLightIndex].FarDist)) / 100.0;
	for(int i=0; i<samples; i++)
	{
		float closestDepth = texture(shadowMapPoint, vec4(fragToLight + gridSamplingDisk[i] * diskRadius, pointLightIndex)).r;
		closestDepth *= u_PointLights[pointLightIndex].FarDist;
		if (currentDepth - bias > closestDepth)
			shadow += 1.0;
	}

	shadow /= float(samples);

	return shadow;
}