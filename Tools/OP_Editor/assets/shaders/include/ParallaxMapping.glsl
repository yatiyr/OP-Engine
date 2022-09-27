

float parallaxShadowMultiplier(in sampler2D heightMap, in vec3 normal, in vec3 lightDir, in vec2 initialTexCoord, in float initialHeight, out float layerHeight, in float TilingFactor, in float HeightFactor)
{
    float shadowMultiplier = 1;

    const float minLayers = 32;
    const float maxLayers = 256;

    if(dot(normal, lightDir) >= 0)
    {
        float numSamplesUnderSurface = 0;
        shadowMultiplier = 0;
        float numLayers = mix(maxLayers, minLayers, abs(dot(normal, lightDir)));
        layerHeight = initialHeight / numLayers;
        if(layerHeight <= 0.0)
            layerHeight = 0.001;
        vec2 texStep = HeightFactor * lightDir.xy / (lightDir.z + 2.5) / numLayers;
        
        // current parameters
        float currentLayerHeight = initialHeight - layerHeight;
        vec2 currentTextureCoords = initialTexCoord + texStep;
        float heightFromTexture = 1.0 - texture(heightMap, currentTextureCoords * TilingFactor).r;
        int stepIndex = 1;

        // while point is below depth 0.0
        while(currentLayerHeight > 0 && !isinf(currentLayerHeight))
        {
            // if point is under the surface
            if(heightFromTexture < currentLayerHeight)
            {
                // calculate partial shadowing factor
                numSamplesUnderSurface += 1;
                float newShadowMultiplier = (currentLayerHeight - heightFromTexture) * (1.0 - stepIndex / numLayers) * (dot(normal, lightDir));
                shadowMultiplier = max(shadowMultiplier, newShadowMultiplier);
            }

            // offset to the next layer
            stepIndex += 1;
            currentLayerHeight -= layerHeight;
            //currentLayerHeight = 0;
            currentTextureCoords += texStep;
            heightFromTexture = 1 - texture(heightMap, currentTextureCoords * TilingFactor).r;
            //break;
        }
        

        // Shadowing factor should be 1 if there were no points under the surface
        if(numSamplesUnderSurface < 1)
        {
            shadowMultiplier = 1;
        }
        else
        {
            shadowMultiplier = 1.0 - shadowMultiplier;
        }
    }

    return shadowMultiplier;
}

vec2 ParallaxMapping(in sampler2D heightMap, in vec3 normal, in vec2 texCoords, in vec3 viewDir, out float parallaxHeight, in float TilingFactor, in float HeightFactor)
{
    if(HeightFactor == 0.0)
        return texCoords;

    // number of depth layers
    const float minLayers = 32;
    const float maxLayers = 256;
    float numLayers = mix(maxLayers, minLayers, abs(dot(normal, viewDir)));
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / (viewDir.z + 2.5) * HeightFactor;
    vec2 deltaTexCoords = P / numLayers;

    // get initial values
    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = 1.0 - texture(heightMap, currentTexCoords * TilingFactor).r;

    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = 1 - texture(heightMap, currentTexCoords * TilingFactor).r;
        // get depth of next layer
        currentLayerDepth += layerDepth;
    }

    // get texture coordinates before collision
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = 1 - texture(heightMap, prevTexCoords * TilingFactor).r - currentLayerDepth + layerDepth;

    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth + 1.0);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    // interpolation of depth values
    parallaxHeight = currentLayerDepth + beforeDepth * weight + afterDepth * (1.0 - weight);

    return finalTexCoords;
}