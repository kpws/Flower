uniform vec2 speed;

void main()
{
	/*gl_FragColor = vec4(sin(gl_TexCoord[0].t*30.0),sin(gl_TexCoord[0].s*30.0),1.0,0.0);*/
	
	gl_FragColor = vec4(
	0.0+sin(gl_TexCoord[0].s*6.0*3.14159)*0.0,
	sin(gl_TexCoord[0].t*6.0*3.14159)*0.0,
	0.0,
	0.0*step(  (sin(gl_TexCoord[0].s*100.0)*sin(gl_TexCoord[0].t*100.0))  ,-0.8)
	);
	
	/*gl_FragColor = vec4(gl_TexCoord[0].t*0.0,  gl_TexCoord[0].s*0.0,  0.0,  0.0);*/
}