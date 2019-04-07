/* File generated with Shader Minifier 1.1.6
 * http://www.ctrl-alt-test.fr
 */
#ifndef POST_INL_
# define POST_INL_

const char *post_frag =
 "#version 130\n"
 "uniform sampler2D o;"
 "out vec4 i;"
 "float hash(float c)"
 "{"
   "return fract(sin(dot(c,12.9898))*43758.5);"
 "}"
 "void main()"
 "{"
   "i=vec4(0);"
   "for(int t=0;t<25;t++)"
     "{"
       "vec2 uv=gl_FragCoord.xy/vec2(1280,720);"
       "float s1=hash(float(t+dot(uv,uv))),s2=hash(float(1-t+dot(uv,uv)));"
       "vec2 f=.01*(-1.+2.*vec2(s1,s2));"
       "i+=vec4(textureLod(o,uv,(.3+.7*s1)*texture(o,(1.+f)*uv).w*8).xyz,1);"
     "}"
   "i/=vec4(25);"
 "}";

#endif // POST_INL_
