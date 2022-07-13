#define SAMPLES_COUNT 64 
#define SAMPLES_COUNT_DIV_2 32 
#define INV_SAMPLES_COUNT (1.0f / SAMPLES_COUNT)   

uniform sampler2D decal;   // decal texture  uniform sampler3D jitter;
// jitter map    uniform sampler2D spot;    // projected spotlight image
uniform sampler2DShadow shadowMap;  // shadow map
uniform float fwidth;  uniform vec2 jxyscale;      // these are passed down from vertex shader
varying vec4 shadowMapPos;
varying vec3 normal;
varying vec2 texCoord; 
varying vec3 lightVec; 
varying vec3 view; 

void main(void)
{    
	float shadow = 0;
	float fsize = shadowMapPos.w * fwidth;   
	vec3 jcoord = vec3(gl_FragCoord.xy * jxyscale, 0);
	vec4 smCoord = shadowMapPos;      // take cheap "test" samples first
	for (int i = 0; i<4; i++) 
	{      
		vec4 offset = texture3D(jitter, jcoord);
		jcoord.z += 1.0f / SAMPLES_COUNT_DIV_2;
		smCoord.xy = offset.xy * fsize + shadowMapPos;
		shadow += texture2DProj(shadowMap, smCoord) / 8;
		smCoord.xy = offset.zw * fsize + shadowMapPos;
		shadow += texture2DProj(shadowMap, smCoord) / 8;
		
	}    
	vec3 N = normalize(normal);
	vec3 L = normalize(lightVec);
	vec3 V = normalize(view);
	vec3 R = reflect(-V, N);      // calculate diffuse dot product
	float NdotL = max(dot(N, L), 0);    // if all the test samples are either zeroes or ones, or diffuse dot  // product is zero, we skip expensive shadow-map filtering
	if ((shadow - 1) * shadow * NdotL != 0)
	{    // most likely, we are in the penumbra 
		 shadow *= 1.0f / 8; // adjust running total      // refine our shadow estimate
		 for (int i = 0; i<SAMPLES_COUNT_DIV_2 - 4; i++)
		 {      
			vec4 offset = texture3D(jitter, jcoord);
			jcoord.z += 1.0f / SAMPLES_COUNT_DIV_2;
			smCoord.xy = offset.xy * fsize + shadowMapPos;
			shadow += texture2DProj(shadowMap, smCoord)* INV_SAMPLES_COUNT;
			smCoord.xy = offset.zw * fsize + shadowMapPos;
			shadow += texture2DProj(shadowMap, smCoord)* INV_SAMPLES_COUNT;
		 }  
	}    // all done Ð modulate lighting with the computed shadow value
	vec3 color = texture2D(decal, texCoord).xyz;
	gl_FragColor.xyz = (color * NdotL + pow(max(dot(R, L), 0), 64)) * shadow * texture2DProj(spot, shadowMapPos) + color * 0.1;
	} 