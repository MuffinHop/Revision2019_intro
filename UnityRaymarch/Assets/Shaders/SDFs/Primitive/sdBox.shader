float sdBox(vec3 p, vec3 b)
{
	vec3 d = abs(p) - b;
	return length(max(d, 0.0))
		+ min(max(d.x, max(d.y, d.z)), 0.0); // remove this line for an only partially signed sdf 
}