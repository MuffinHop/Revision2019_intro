// Capsule: A Cylinder with round caps on both sides
float fCapsule(vec3 p, float r, float c) {
	return mix(length(p.xz) - r, length(vec3(p.x, abs(p.y) - c, p.z)) - r, step(c, abs(p.y)));
}