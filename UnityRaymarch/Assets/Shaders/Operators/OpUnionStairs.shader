// The "Stairs" flavour produces n-1 steps of a staircase:
// much less stupid version by paniq
float fOpUnionStairs(float a, float b, float r, float n) {
	float s = r / n;
	float u = b - r;
	return min(min(a, b), 0.5 * (u + a + abs((mod(u - a + s, 2 * s)) - s)));
}