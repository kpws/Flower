uniform sampler2D tex;
uniform vec2 speed;

void main()
{
	vec4 val = texture2D(tex,gl_TexCoord[0].st);
	
	gl_FragColor = vec4(speed.xy*0.3,val.b,0.0);
	
	/*gl_FragColor = vec4((val.x*15.0+0.2)/16.0,val.y,val.b*0.5,1.0);*/
	
	/*gl_FragColor=val;
	gl_FragColor.a=1.0;*/
	
	/*float increase=0.1;
	gl_FragColor = vec4((val.r*val.b)/(val.b+increase),(val.g*val.b)/(val.b+increase) ,val.b+increase,1.0);*/
	
	/*float speed=1.0;
	gl_FragColor = vec4(val.r+(speed-val.r)*0.0002,val.g,val.b,1.0);
	*/
}