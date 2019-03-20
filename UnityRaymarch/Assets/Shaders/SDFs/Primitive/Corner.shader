// Endless "corner"
float fCorner(vec2 p) {
	return length(max(p, vec2(0))) + vmax(min(p, vec2(0)));
}
