#include "../Modifiers/R45.shader"
#include "../Modifiers/Mod1.shader"
float fOpDifferenceColumns(float a, float b, float r, float n) {
	a = -a;
	float m = min(a, b);
	//avoid the expensive computation where not needed (produces discontinuity though)
	if ((a < r) && (b < r)) {
		vec2 p = vec2(a, b);
		float columnradius = r * sqrt(2) / n / 2.0;
		columnradius = r * sqrt(2) / ((n - 1) * 2 + sqrt(2));

		pR45(p);
		p.y += columnradius;
		p.x -= sqrt(2) / 2 * r;
		p.x += -columnradius * sqrt(2) / 2;

		if (mod(n, 2) == 1) {
			p.y += columnradius;
		}
		pMod1(p.y, columnradius * 2);

		float result = -length(p) + columnradius;
		result = max(result, p.x);
		result = min(result, a);
		return -min(result, b);
	}
	else {
		return -m;
	}
}