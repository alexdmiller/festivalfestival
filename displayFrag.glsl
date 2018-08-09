

#ifdef GL_ES
precision highp float;
#endif

#define PROCESSING_TEXTURE_SHADER

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;
uniform sampler2D ppixels;

void main()
{
	//vec2 uv = gl_FragCoord.xy / resolution.xy;
    //ivec2 uv = ivec2(gl_FragCoord.xy*0.01*mouse.xy);
    ivec2 uv = ivec2(gl_FragCoord.xy);
	//fragColor = vec4( 65.0*pow(texture(iChannel0, uv).rgb, vec3(6.0)), 1.0);
    //fragColor = vec4( 1.0*pow(texture(iChannel0, uv).rgb, vec3(1.0)), 1.0);
    vec3 c = texelFetch(ppixels, uv, 0).rgb;
    float l = length(c);
    //fragColor = vec4( vec3(5.0*pow(l,18.0)),1.0);
    //fragColor = vec4( vec3(1.0*pow(l,1.0)),1.0);
    vec3 c2 = vec3( 0.5*sin(2.7*l)+0.5, 0.5*sin(1.7*l)+0.5, 0.5*sin(1.9*l)+0.5);
    
    // Converts range 0.0-1.0 to a full rainbow of colors;
    //l = fragCoord.x/iResolution.x;
    
    vec3 c3 = vec3(0.0);
    float a= pow((1.-l)/0.8, 1.0);
	int x=int(floor(a));
	float y = /*floor*/(1.0*(a-float(x)));
    //*
	switch(x)
	{
 	  case 0: c3.r=1.;c3.g=y;c3.b=0.;break;
  	  case 1: c3.r=1.-y;c3.g=1.;c3.b=0.;break;
  	  case 2: c3.r=0.;c3.g=1.;c3.b=y;break;
  	  case 3: c3.r=0.;c3.g=1.-y;c3.b=1.;break;
  	  case 4: c3.r=y;c3.g=0.;c3.b=1.;break;
  	  case 5: c3.r=1.;c3.g=0.;c3.b=1.;break;
	}
	//*/
    
    //fragColor = vec4( vec3(1.0*pow(c3,vec3(1.0))),1.0);
	gl_FragColor = vec4(vec3(l),1.0);//vec3(0.5*sin(6.0*l+0.3*time)+0.5),1.0);
}