uniform sampler2D flowTex;
uniform int res;
	
void main()
{
	float d=1.0/float(res);
	float d2=2.0*d;


	
	float rot=(texture2D(flowTex,gl_TexCoord[0].st+vec2(d,0.0)).y-texture2D(flowTex,gl_TexCoord[0].st+vec2(-d,0.0)).y-texture2D(flowTex,gl_TexCoord[0].st+vec2(0.0,d)).x+texture2D(flowTex,gl_TexCoord[0].st+vec2(0.0,-d)).x)/d2;
	gl_FragColor=vec4(rot*0.03,rot*0.03,-rot*0.03,1.0);

	
	
}
