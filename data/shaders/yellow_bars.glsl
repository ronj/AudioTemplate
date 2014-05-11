uniform float time;
uniform vec2 resolution;
uniform sampler2D texture;

vec3 view(vec2 p)
{
	float r = resolution.y / resolution.x;
	float x = p.x-fract(p.x*80.)/80.;
	float h = texture2D(texture,vec2(x,0)).x;
	
	if (p.y <= h-fract(h*80.*r)/(80.*r)) {
		float py = p.y - fract(p.y*80.*r)/(80.*r);
		return vec3(h,h,py)*1.5;
	}
	return vec3(0.,0.,0.0);
	
}

void main(void)
{
	vec2 p = gl_FragCoord.xy/resolution.xy;
	gl_FragColor = vec4(view(p), 1.);
}