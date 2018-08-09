//

#ifdef GL_ES
precision highp float;
#endif

#define PROCESSING_COLOR_SHADER

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;
uniform sampler2D ppixels;
uniform float blur1;
uniform float comp;
uniform float something;

void main()
{
    ivec2 uv = ivec2(gl_FragCoord.xy);

    if (time < 0.3) {
        gl_FragColor = vec4(sin(gl_FragCoord.x),
		                    sin(gl_FragCoord.y),
							cos(gl_FragCoord.x),1.0);
    } else {

        // Center pixel

        vec3 c = texelFetch(ppixels,uv,0).rgb;

        //// 3x3 ////

        // Adjacent
        vec3 u = texelFetch(ppixels, uv + ivec2(0, 1), 0).rgb;
        vec3 d = texelFetch(ppixels, uv + ivec2(0, -1),0).rgb;
        vec3 l = texelFetch(ppixels, uv + ivec2(-1, 0),0).rgb;
        vec3 r = texelFetch(ppixels, uv + ivec2( 1, 0),0).rgb;

        // Diagonal
        vec3 ul = texelFetch(ppixels, uv + ivec2(-1, 1),0).rgb;
        vec3 dl = texelFetch(ppixels, uv + ivec2(-1, -1),0).rgb;
        vec3 ur = texelFetch(ppixels, uv + ivec2(1,  1),0).rgb;
        vec3 dr = texelFetch(ppixels, uv + ivec2(1, -1),0).rgb;

        //// 5x5 ////

        vec3 fa = texelFetch(ppixels, uv + ivec2(-2, 2),0).rgb;
        vec3 fb = texelFetch(ppixels, uv + ivec2(-2,  1),0).rgb;
        vec3 fc = texelFetch(ppixels, uv + ivec2(-2,  0),0).rgb;
        vec3 fd = texelFetch(ppixels, uv + ivec2(-2, -1),0).rgb;
        vec3 fe = texelFetch(ppixels, uv + ivec2(-2,-2),0).rgb;

        vec3 ff = texelFetch(ppixels, uv + ivec2(2, 2),0).rgb;
        vec3 fg = texelFetch(ppixels, uv + ivec2(2, 1),0).rgb;
        vec3 fh = texelFetch(ppixels, uv + ivec2(2, 0),0).rgb;
        vec3 fi = texelFetch(ppixels, uv + ivec2(2, -1),0).rgb;
        vec3 fj = texelFetch(ppixels, uv + ivec2(2,-2),0).rgb;

        vec3 fk = texelFetch(ppixels, uv + ivec2(-1, 2),0).rgb;
        vec3 fl = texelFetch(ppixels, uv + ivec2( 0, 2),0).rgb;
        vec3 fm = texelFetch(ppixels, uv + ivec2( 1, 2),0).rgb;

        vec3 fn = texelFetch(ppixels, uv + ivec2(-1,-2),0).rgb;
        vec3 fo = texelFetch(ppixels, uv + ivec2( 0,-2),0).rgb;
        vec3 fp = texelFetch(ppixels, uv + ivec2( 1,-2),0).rgb;

        vec3 farthest = fa+fe+ff+fi;
        vec3 mid = fb+fd+fg+fi+fk+fm+fn+fp;
        vec3 close = fc+fh+fl+fo;

        vec3[] kernel = vec3[]
              ( fa, fk, fl, fm, ff,
               	fb, ul,  u, ur, fg,
                fc,  l,  c,  r, fh,
                fd, dl,  d, dr, fi,
                fe, fn, fc, fp, fj );

        vec3 blur = blur1 * c + 0.095238 * (u+d+l+r) + 0.058608 * (ul+dl+ur+dr)
                + 0.025641 * close + 0.014652 * mid + 0.003663 * farthest;

      //  vec3 wild = kernel[ int(6.0*length(kernel[ int(6.0*length(kernel[12]))+11]))+11];


        vec3 wild2 = vec3(0.0);
        for (int i = 0; i < 25; i++ ) {
        	wild2 += 0.5*normalize(cross(wild2,kernel[i]-comp)+comp);
        }
      // wild2 /= 5.0;
     //   wild2 *= 5.0;//iMouse.x*0.018+0.6;

       // wild2 = mix(wild2, c, 0.001);
        //float m = sqrt(mouse.y/resolution.y);
        vec2 coords = gl_FragCoord.xy / resolution.xy - 0.5;
        coords.x *= resolution.x/resolution.y;
        float m = something*distance(coords, vec2(0.0));

        wild2 = mix(mix(blur,wild2,m), blur, m );

       // fragColor = vec4(wild2,1.0);
        gl_FragColor = vec4( mix(wild2,
                             (vec3(0.0)), 0.75)/comp ,1.0);

    }

}