// Mirror at an axis-aligned plane which is at a specified distance <dist> from the origin.
float pMirror(inout float p, float dist) {
	float s = sgn(p);
	p = abs(p) - dist;
	return s;
}