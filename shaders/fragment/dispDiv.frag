uniform sampler2D flowTex;
uniform int res;
	
void main()
{	
	float d=1.0/float(res);
	float d2=2.0*d;

	float div=(texture2D(flowTex,gl_TexCoord[0].st+vec2(d,0.0)).x-texture2D(flowTex,gl_TexCoord[0].st+vec2(-d,0.0)).x+texture2D(flowTex,gl_TexCoord[0].st+vec2(0.0,d)).y-texture2D(flowTex,gl_TexCoord[0].st+vec2(0.0,-d)).y)/d2;
	
	gl_FragColor=vec4(div*0.1,-div*0.1,div*0.1,1.0);
}
