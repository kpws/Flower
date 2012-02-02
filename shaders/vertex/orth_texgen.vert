void main()
{
	gl_FrontColor = gl_Color;
	gl_TexCoord[0] = ftransform()*0.5+0.5/*gl_ModelViewMatrix*gl_Vertex*/;
	gl_Position = ftransform();
}
