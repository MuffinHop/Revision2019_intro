#include "../Modifiers/R45.shader"
#include "../Modifiers/Mod1.shader"
// The "Columns" flavour makes n-1 circular columns at a 45 degree angle:
float fOpUnionColumns(float a, float b, float r, float n) {
	if ((a < r) && (b < r)) {
		vec2 p = vec2(a, b);
		float columnradius = r * sqrt(2) / ((n - 1) * 2 + sqrt(2));
		pR45(p);
		p.x -= sqrt(2) / 2 * r;
		p.x += columnradius * sqrt(2);
		if (mod(n, 2) == 1) {
			p.y += columnradius;
		}
		// At this point, we have turned 45 degrees and moved at a point on the
		// diagonal that we want to place the columns on.
		// Now, repeat the domain along this direction and place a circle.
		pMod1(p.y, columnradius * 2);
		float result = length(p) - columnradius;
		result = min(result, p.x);
		result = min(result, a);
		return min(result, b);
	}
	else {
		return min(a, b);
	}
}
