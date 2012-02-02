uniform sampler2D flowTex,oldInkTex;
uniform int res,iterations;

void main()
{	
	float d=1.0/float(res);
	float d2=7.0*d;
	const float k=0.02;
	float dt=0.002*float(iterations);
	
	
	
	/*vec2 v=texture2D(flowTex,gl_TexCoord[0].st).rg;
	
	v=texture2D(flowTex,gl_TexCoord[0].st-v*dt*0.5).rg;
	*/
	/*gl_FragColor.rgb=texture2D(oldInkTex,gl_TexCoord[0].st-v*dt).rgb;*/
	
	gl_FragColor.rgb=texture2D(oldInkTex,gl_TexCoord[0].st-texture2D(flowTex,gl_TexCoord[0].st-texture2D(flowTex,gl_TexCoord[0].st).rg*dt*0.0).rg*dt).rgb;
	
	
	/*gl_FragColor.rg+=(gl_FragColor.rg-vec2(0.5))*.0002;*/
	/*float hs=0.00001;*/
	
	/*if(gl_FragColor.r>2.0*abs(gl_TexCoord[0].s-0.5))
		gl_FragColor.r-=hs;
	else if(gl_FragColor.r<2.0*abs(gl_TexCoord[0].s-0.5))
		gl_FragColor.r+=hs;	
		
	if(gl_FragColor.g>2.0*abs(gl_TexCoord[0].t-0.5))
		gl_FragColor.g-=hs;
	else if(gl_FragColor.g<2.0*abs(gl_TexCoord[0].t-0.5))
		gl_FragColor.g+=hs;	*/
		
	/*if(gl_FragColor.r>gl_TexCoord[0].s)
		gl_FragColor.r-=hs;
	else if(gl_FragColor.r<gl_TexCoord[0].s)
		gl_FragColor.r+=hs;	
		
	if(gl_FragColor.g>gl_TexCoord[0].t)
		gl_FragColor.g-=hs;
	else if(gl_FragColor.g<gl_TexCoord[0].t)
		gl_FragColor.g+=hs;*/
		
	/*gl_FragColor.r=(
	texture2D(oldInkTex,gl_TexCoord[0].st+vec2(d2,0)-texture2D(flowTex,gl_TexCoord[0].st+vec2(d2,0)).rg*dt).r+
	texture2D(oldInkTex,gl_TexCoord[0].st+vec2(-d2,0)-texture2D(flowTex,gl_TexCoord[0].st+vec2(-d2,0)).rg*dt).r+
	texture2D(oldInkTex,gl_TexCoord[0].st+vec2(0,d2)-texture2D(flowTex,gl_TexCoord[0].st+vec2(0,d2)).rg*dt).r+
	texture2D(oldInkTex,gl_TexCoord[0].st+vec2(0,-d2)-texture2D(flowTex,gl_TexCoord[0].st+vec2(0,-d2)).rg*dt).r)*0.249;
	*/
	
}