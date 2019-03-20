// Same as above, but in two dimensions (an endless box)
float fBox2Cheap(vec2 p, vec2 b) {
	return vmax(abs(p) - b);
}
float fBox2(vec2 p, vec2 b) {
	vec2 d = abs(p) - b;
	return length(max(d, vec2(0))) + vmax(min(d, vec2(0)));
}