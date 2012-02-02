uniform sampler2D tex,mattex,inktex;
uniform int res;

void main()
{	
	float d=1.0/float(res);
	float d2=2.0*d;
	const float k=0.02;
	const float dt=0.002;
	const float my=0.00005;
	
	/*if(gl_TexCoord[0].s<=d2)
	{
		gl_FragColor=vec4(-0.07,0.0,0.0,0.0);/*texture2D(tex,gl_TexCoord[0].st+vec2(3.0*d,0.0));
		return;
	}
	
	if(gl_TexCoord[0].s>=1.0-d2)
	{
		gl_FragColor=vec4(-0.07,0.0,0.0,0.0);/*texture2D(tex,gl_TexCoord[0].st-vec2(3.0*d,0.0));
		return;
	}*/
	
	vec4 mat = texture2D(mattex,gl_TexCoord[0].st);
	vec4 leftval = texture2D(tex,gl_TexCoord[0].st+vec2(-d,0.0));
	vec4 rightval = texture2D(tex,gl_TexCoord[0].st+vec2(d,0.0));
	vec4 downval = texture2D(tex,gl_TexCoord[0].st+vec2(0.0,-d));
	vec4 upval = texture2D(tex,gl_TexCoord[0].st+vec2(0.0,d));
	
	float drho_dt=-(rightval.x*(rightval.b+2.0)-leftval.x*(leftval.b+2.0)+upval.y*(upval.b+2.0)-downval.y*(downval.b+2.0))/d2;
	vec2 b=vec2(0.0);
	
	if(mat.r>0.0)
	{
		if(mat.r<0.15)
		{
			float rho=texture2D(tex,gl_TexCoord[0].st).b+2.0;
			
			/*drho_dt-=(rho-2.0);*/
			rho+=drho_dt*dt;
			if(rho<0.1)rho=0.1;
		
			gl_FragColor=vec4(0.0,0.0,rho-2.0,0.0);
			return;
		}
		if(mat.r<0.25)
		{
			/*if(texture2D(mattex,gl_TexCoord[0].st+vec2(d2,0)).r>0.0)
			{
				gl_FragColor=vec4(0.0, 0.0, 0.0, 0.0 );
				return;
			}
			else
			{
				gl_FragColor=vec4(-0.1, 0.0, 0.0, 0.0 );
				return;
			}*/
		
			b+=vec2(1.0*(texture2D(tex,gl_TexCoord[0].st).b+2.0),0.0);
		}
	}

	vec4 val = texture2D(tex,gl_TexCoord[0].st);
	
	if(val.a==1.0)
	{
		float rho=texture2D(tex,gl_TexCoord[0].st).b+2.0;
			
		/*drho_dt-=(rho-2.0);*/
		rho+=drho_dt*dt;
		if(rho<0.1)rho=0.1;
	
		gl_FragColor=vec4(val.r,val.g,rho-2.0,0.0);
		return;
	}
	
	vec3 leftleftval = texture2D(tex,gl_TexCoord[0].st+vec2(-d2,0.0)).rgb;
	vec3 rightrightval = texture2D(tex,gl_TexCoord[0].st+vec2(d2,0.0)).rgb;
	vec3 downdownval = texture2D(tex,gl_TexCoord[0].st+vec2(0.0,-d2)).rgb;
	vec3 upupval = texture2D(tex,gl_TexCoord[0].st+vec2(0.0,d2)).rgb;
	
	vec3 leftupval = texture2D(tex,gl_TexCoord[0].st+vec2(-d,d)).rgb;
	vec3 rightupval = texture2D(tex,gl_TexCoord[0].st+vec2(d,d)).rgb;
	vec3 leftdownval = texture2D(tex,gl_TexCoord[0].st+vec2(-d,-d)).rgb;
	vec3 rightdownval = texture2D(tex,gl_TexCoord[0].st+vec2(d,-d)).rgb;
	

	
	/*drho_dt-=val.b*0.3;*/
	val.b+=drho_dt*dt;
	/*if(val.b<-1.9)val.b=-1.9;*/

	/*vec2 grad_ux=vec2(-rightrightval.x+8.0*rightval.x-8.0*leftval.x+leftleftval.x,-upupval.x+8.0*upval.x-8.0*downval.x+downdownval.x)/(12.0*d);
	vec2 grad_uy=vec2(-rightrightval.y+8.0*rightval.y-8.0*leftval.y+leftleftval.y,-upupval.y+8.0*upval.y-8.0*downval.y+downdownval.y)/(12.0*d);
	*/
	vec2 grad_ux=vec2(rightval.x-leftval.x,upval.x-downval.x)/d2;
	vec2 grad_uy=vec2(rightval.y-leftval.y,upval.y-downval.y)/d2;
	
	vec2 laplace_u=(leftval.xy+rightval.xy+upval.xy+downval.xy+(leftupval.xy+leftdownval.xy+rightupval.xy+rightdownval.xy)/2.0-6.0*val.xy)/(d*d);
	
	vec2 grad_div_u=vec2(
						rightrightval.x+leftleftval.x+rightupval.y-rightdownval.y-leftupval.y+leftdownval.y-2.0*val.x,
						upupval.y+downdownval.y+rightupval.x-leftupval.x-rightdownval.x+leftdownval.x-2.0*val.y
						)/(d2*d2);
						
	/*b+=vec2(0.0,mat.r*9.9);*/
	
	float rot=(rightval.y-leftval.y-upval.x+downval.x)/d2;
	/*psi = riktningen beloppet pa rotationen okar mest i*/
	vec2 psi=normalize(
						vec2(
							(rightrightval.y-val.y)      -(val.y-leftleftval.y)      -(rightupval.x-leftupval.x)+(rightdownval.x-leftdownval.x),
							(rightupval.y-rightdownval.y)-(leftupval.y-leftdownval.y)-(upupval.x-val.x)         +(val.x-downdownval.x)
							)*sign(rot)
						);
							
	float eps=45.0;
	b+=eps*vec2(psi.y,-psi.x)*rot*d;
	
	/*vec3 ink=texture2D(inktex,gl_TexCoord[0].st).rgb;
	b.y+=ink.r*0.2;*/ /*-(ink.g+ink.b)*0.5;*/
	/*b.x+=ink.g*0.86602540378-ink.b*0.86602540378;*/
	
	/*vec2 homedir=(texture2D(inktex,gl_TexCoord[0].st).rg-gl_TexCoord[0].st);
	
	if(homedir.x<-0.5)homedir.x+=1.0;
	else if(homedir.x>0.5)homedir.x-=1.0;
	if(homedir.y<-0.5)homedir.y+=1.0;
	else if(homedir.y>0.5)homedir.y-=1.0;
	if(dot(homedir,homedir)<0.1*0.1)
	b+=(texture2D(inktex,gl_TexCoord[0].st).rg-gl_TexCoord[0].st)*0.5;*/
	
	vec2 uadvection=texture2D(tex,gl_TexCoord[0].st-val.rg*dt).rg-val.rg;
	
	vec2 du_dt=
		(
			b
			-k*vec2(rightval.b-leftval.b,upval.b-downval.b)/d2
			+my*(laplace_u+grad_div_u/3.0)
		)/(val.b+2.0);
	

	gl_FragColor=vec4(val.rg+du_dt*dt+uadvection,val.b+drho_dt*dt*0.0,0.0);
	
	/*if(gl_FragColor.b<-1.9)gl_FragColor.b=-1.9;*/
	
	
	float maxspeed=d/dt*0.3;
	if(dot(gl_FragColor.rg,gl_FragColor.rg)>maxspeed*maxspeed)
		gl_FragColor.rg*=maxspeed/length(gl_FragColor.rg);
	

	
	/*if(gl_TexCoord[0].t<=d2 || gl_TexCoord[0].t>=1.0-d2)
	{
		gl_FragColor.b=2.0;
		gl_FragColor.rg=0.0;
		return;
	}*/
}
