// Repeat the domain only in positive direction. Everything in the negative half-space is unchanged.
float pModSingle1(inout float p, float size) {
	float halfsize = size * 0.5;
	float c = floor((p + halfsize) / size);
	if (p >= 0)
		p = mod(p + halfsize, size) - halfsize;
	return c;
}