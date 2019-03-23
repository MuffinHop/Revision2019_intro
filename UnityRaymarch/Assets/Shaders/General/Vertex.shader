#ifdef VERTEX     
out vec4 positionition_in_object_coordinates;
void main()
{
	positionition_in_object_coordinates = gl_Vertex;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
#endif