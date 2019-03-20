float sdRoundCone(in vec3 p, in float r1, float r2, float h)
{
	vec2 q = vec2(length(p.xz), p.y);

	float b = (r1 - r2) / h;
	float a = sqrt(1.0 - b * b);
	float k = dot(q, vec2(-b, a));

	if (k < 0.0) return length(q) - r1;
	if (k > a*h) return length(q - vec2(0.0, h)) - r2;

	return dot(q, vec2(a, b)) - r1;
}