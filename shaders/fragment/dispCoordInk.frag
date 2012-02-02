uniform sampler2D inkTex,flowerTex;
uniform int res;
	
void main()
{
	
	/*gl_FragColor.rgba=atan((texture2D(inkTex,gl_TexCoord[0].st)-0.5)*10.0)/3.1415*2.0+1.0;*/
	
	/*gl_FragColor.rgba=0.5+0.5*sign(sin(texture2D(inkTex,gl_TexCoord[0].st)*100.0));*/
/*	gl_FragColor.r=atan(gl_FragColor.r*10.0);*/

	gl_FragColor.rgba=texture2D(flowerTex,texture2D(inkTex,gl_TexCoord[0].st).rg);
	
}

