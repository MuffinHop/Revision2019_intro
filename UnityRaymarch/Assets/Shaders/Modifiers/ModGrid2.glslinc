// Same, but mirror every second cell at the diagonal as well
vec2 pModGrid2(inout vec2 p, vec2 size) {
	vec2 c = floor((p + size * 0.5) / size);
	p = mod(p + size * 0.5, size) - size * 0.5;
	p *= mod(c, vec2(2)) * 2 - vec2(1);
	p -= size / 2;
	if (p.x > p.y) p.xy = p.yx;
	return floor(c / 2);
}