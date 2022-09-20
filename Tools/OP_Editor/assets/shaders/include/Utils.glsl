
float linstep(float low, float high, float v)
{
	return clamp((v-low) / (high - low), 0.0, 1.0);
}

float PerspectiveProjDepthLinearize(float depth, float near, float far)
{
	float z = depth * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near - z * (far - near));
}