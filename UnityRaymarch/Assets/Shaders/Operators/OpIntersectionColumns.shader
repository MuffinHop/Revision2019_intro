#include "OpDifferenceColumns.shader"
float fOpIntersectionColumns(float a, float b, float r, float n) {
	return fOpDifferenceColumns(a, -b, r, n);
}