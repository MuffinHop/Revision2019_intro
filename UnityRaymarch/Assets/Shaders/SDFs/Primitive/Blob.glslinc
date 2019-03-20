// Blobby ball object. You've probably seen it somewhere. This is not a correct distance bound, beware.
float fBlob(vec3 p) {
	p = abs(p);
	if (p.x < max(p.y, p.z)) p = p.yzx;
	if (p.x < max(p.y, p.z)) p = p.yzx;
	float b = max(max(max(
		dot(p, normalize(vec3(1, 1, 1))),
		dot(p.xz, normalize(vec2(PHI + 1, 1)))),
		dot(p.yx, normalize(vec2(1, PHI)))),
		dot(p.xz, normalize(vec2(1, PHI))));
	float l = length(p);
	return l - 1.5 - 0.2 * (1.5 / 2)* cos(min(sqrt(1.01 - b / l)*(PI / 0.25), PI));
}