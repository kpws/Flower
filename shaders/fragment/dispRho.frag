uniform sampler2D flowTex;
uniform int res;
	
void main()
{
	float d=1.0/float(res);
	float d2=2.0*d;
	
	float rhoDiffHalf = texture2D(flowTex,gl_TexCoord[0].st).b/2.0;
	

	
	gl_FragColor.r=rhoDiffHalf;
	gl_FragColor.g=0.0;
	gl_FragColor.b=-rhoDiffHalf;
	
	
}
