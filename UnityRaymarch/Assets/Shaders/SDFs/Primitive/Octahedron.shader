float fOctahedron(vec3 p, float r, float e) {
	return fGDF(p, r, e, 3, 6);
}
float fTruncatedOctahedron(vec3 p, float r, float e) {
	return fGDF(p, r, e, 0, 6);
}
float fOctahedron(vec3 p, float r) {
	return fGDF(p, r, 3, 6);
}
float fTruncatedOctahedron(vec3 p, float r) {
	return fGDF(p, r, 0, 6);
}