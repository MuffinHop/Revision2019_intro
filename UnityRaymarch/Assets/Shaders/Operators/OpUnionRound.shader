// The "Round" variant uses a quarter-circle to join the two objects smoothly:
float fOpUnionRound(float a, float b, float r) {
	vec2 u = max(vec2(r - a, r - b), vec2(0));
	return max(r, min(a, b)) - length(u);
}