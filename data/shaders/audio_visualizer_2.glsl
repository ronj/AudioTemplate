/*by musk License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

	Audio Visualizaer using my graphing tool. https://www.shadertoy.com/view/4dS3WG

*/

uniform float time;
uniform vec2 resolution;
uniform sampler2D texture;

#define quality 8
#define smoothness 1.6
#define grid
//#define subpixel
//#define reverseLCD

float f(float x)
{
	float q = .0;
		
	if (x<0.0)
	{
		x = x/7.0+1.0;
		q = texture2D(texture,vec2(x,1.0)).y-.5;
		q*= texture2D(texture,vec2(0.0,0.0)).y*2.0;
	}
	else
	{
		x=x/7.0;
		x=pow(x*.82+.18,4.0);
		q = texture2D(texture,vec2(x,0.0)).y-.5;
	}
	q*=4.0;
	return q;
}

float hash(float x)
{
	return fract(sin(x*164437.36831)*13217.321); //decent hash for noise generation
}

void main(void)
{
	vec2 uv = gl_FragCoord.xy / resolution.xy-.5;
	uv.x = uv.x*resolution.x/resolution.y;
	uv*=2.0;
	float pixel = 4.0/resolution.y;
	
	uv*=4.0; 
		
	vec3 c = vec3(.0);
	
	for (float fi=.0; fi<1.0; fi += 1.0/float(quality))
	{
		#ifdef subpixel
		for (int i=0; i<3; i++)
		{
			float x = uv.x + hash(uv.y*41.0+fi)*pixel*float(smoothness)*.3333 + float(i)*pixel*.3333;
			float y = uv.y + hash(uv.x*41.0+fi)*pixel*float(smoothness);
		
			float s0 = f(x-pixel);
			float s1 = f(x+pixel);
			
			if (s0-pixel <= y && y < s1+pixel || s1-pixel <= y && y < s0+pixel)
			{
				#ifdef reverseLCD
				c[2-i] += 1.0;
				#else
				c[i] += 1.0;
				#endif
			}
		}
		#else 
		float x = uv.x + hash(uv.y*41.0+fi)*pixel*float(smoothness);
		float y = uv.y + hash(uv.x*41.0+fi)*pixel*float(smoothness);
		
		float s0 = f(x-pixel);
		float s1 = f(x+pixel);
			
		if (s0-pixel <= y && y < s1+pixel || s1-pixel <= y && y < s0+pixel)
		{
			c += vec3(1.0);
		}
		#endif
	}
	
	c/=float(quality);
	
	#ifdef grid
	c += max(.0,1.0-abs(mod(uv.x+.5,1.0)-.5)/pixel*0.5)*.1/(.5+abs(uv.x)*.5);
	c += max(.0,1.0-abs(mod(uv.y+.5,1.0)-.5)/pixel*0.5)*.1/(.5+abs(uv.y)*.5);
	#endif
	
	gl_FragColor = vec4(pow(c,vec3(.6)),1.0);
}
