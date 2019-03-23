// Same, but mirror every second cell so all boundaries match
vec2 pModMirror2(inout vec2 p, vec2 size) {
	vec2 halfsize = size * 0.5;
	vec2 c = floor((p + halfsize) / size);
	p = mod(p + halfsize, size) - halfsize;
	p *= mod(c, vec2(2)) * 2 - vec2(1);
	return c;
}