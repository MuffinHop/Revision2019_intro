// Reflect space at a plane
float pReflect(inout vec3 p, vec3 planeNormal, float offset) {
	float t = dot(p, planeNormal) + offset;
	if (t < 0) {
		p = p - (2 * t)*planeNormal;
	}
	return sgn(t);
}