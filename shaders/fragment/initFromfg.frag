uniform sampler2D fgTex;

void main()
{
	/*if( texture2D(fgTex,gl_TexCoord[0].st).a<0.5)
		gl_FragColor = vec4(
	0.0,
	0.0,
	0.0,
	0.0
	);
	else
		gl_FragColor = vec4(
	0.1,
	0.0,
	0.0,
	0.0
	);*/
	
	gl_FragColor = vec4(
	gl_TexCoord[0].s,
	gl_TexCoord[0].t,
	0.0,
	0.0
	);
	
	/*gl_FragColor=texture2D(fgTex,gl_TexCoord[0].st);*/
}