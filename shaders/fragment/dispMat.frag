uniform sampler2D matTex;
uniform int res;
	
void main()
{
	float d=1.0/float(res);
	float d2=2.0*d;

	float mat=texture2D(matTex,gl_TexCoord[0].st).r;
		
	if(mat>0.07 && mat < 0.13)
		gl_FragColor=vec4(0.2,0.2,0.2,0.0);
	else if(mat>0.11 && mat <0.18)
		gl_FragColor=vec4(0.0,1.0,0.0,0.0);
	else
		discard;
}
