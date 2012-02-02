uniform sampler2D tex;
uniform vec2 speed,rotspeed,midpos;

void main()
{
	gl_FragColor = vec4(speed.xy+rotspeed.x*vec2(midpos.y-gl_TexCoord[0].t,gl_TexCoord[0].s-midpos.x),texture2D(tex,gl_TexCoord[0].st).b,1.0);
	
	/*gl_FragColor=vec4((gl_TexCoord[0].st-midpos)*10.0,texture2D(tex,gl_TexCoord[0].st).b,1.0);*/	
}