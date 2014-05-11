uniform float time;
uniform vec2 resolution;
uniform sampler2D texture;

#define PI  3.14159
#define EPS .001

#define T .03  // Thickness
#define W 2.   // Width
#define A .09  // Amplitude
#define V 1.   // Velocity

void main( void ) {
  vec2 c = gl_FragCoord.xy / resolution.xy;
  vec4 s = texture2D(texture, c * .5);
  c = vec2(0., A*s.y*sin((c.x*W+time*V)* 2.5)) + (c*2.-1.);
  float g = max(abs(s.y/(pow(c.y, 2.1*sin(s.x*PI))))*T,
                abs(.1/(c.y+EPS)));
  gl_FragColor = vec4(g*g*s.y*.6, g*s.w*.44, g*g*.7, 1.);
}
