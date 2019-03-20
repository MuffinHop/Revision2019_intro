#include "Mirror.shader"
// Mirror in both dimensions and at the diagonal, yielding one eighth of the space.
// translate by dist before mirroring.
vec2 pMirrorOctant(inout vec2 p, vec2 dist) {
	vec2 s = sgn(p);
	pMirror(p.x, dist.x);
	pMirror(p.y, dist.y);
	if (p.y > p.x)
		p.xy = p.yx;
	return s;
}