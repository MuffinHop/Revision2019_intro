// Difference can be built from Intersection or Union:
float fOpDifferenceChamfer(float a, float b, float r) {
	return fOpIntersectionChamfer(a, -b, r);
}
