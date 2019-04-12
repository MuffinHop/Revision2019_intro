/* File generated with Shader Minifier 1.1.6
 * http://www.ctrl-alt-test.fr
 */
#ifndef FRAGMENT_INL_
# define FRAGMENT_INL_

const char *fragment_frag =
 "#version 130\n"
 "uniform sampler2D _TextTex;"
 "uniform float _iTime;"
 "uniform vec4 _iResolution,_DirectionalLight,_DirectionalLightColor,_PointLightPosition,_PointLightColor,_CameraPosition,_CameraLookAt,_CameraUp;"
 "uniform float _FOV,_Distance,_LensCoeff,_MaxCoC,_RcpMaxCoC,_MarchMinimum,_FarPlane,_Step,fogDensity,_TextId;\n"
 "#define maxItersGlobal 48\n"
 "#define ENABLE_FOG\n"
 "#define ENABLE_REFLECTIONS\n"
 "#define ENABLE_SHADOWS\n"
 "#define ENABLE_TRANSPARENCY\n"
 "#define ENABLE_AO\n"
 "#define DOUBLE_SIDED_TRANSPARENCY\n"
 "#define saturate(x)clamp(x,0,1)\n"
 "#define FRAGMENT_P lowp\n"
 "const float PI=3.14159,DEG_TO_RAD=PI/180.,TWOPI=PI*2.,TAU=PI*2.,PHI=sqrt(5)*.5+.5,farClip=32.,noTransparency=-1.,transparencyInformation=1.,emptyInformation=0.;struct Trace{vec3 origin;vec3 direction;float startdistanc;float length;};struct Surface{vec3 normal;vec3 reflection;vec3 subsurface;};struct Material{float reflectionCoefficient;float smoothness;float transparency;float reflectindx;vec3 albedo;};struct Shading{vec3 diffuse;vec3 specular;};struct ContactInfo{vec3 position;float distanc;vec3 id;};struct PointLight{vec3 position;vec3 color;};struct DirectionLight{vec3 direction;vec3 color;};"
 "const float kHashScale1=443.898;"
 "const vec3 GDFVectors[19]=vec3[](normalize(vec3(1,0,0)),normalize(vec3(0,1,0)),normalize(vec3(0,0,1)),normalize(vec3(1,1,1)),normalize(vec3(-1,1,1)),normalize(vec3(1,-1,1)),normalize(vec3(1,1,-1)),normalize(vec3(0,1,PHI+1)),normalize(vec3(0,-1,PHI+1)),normalize(vec3(PHI+1,0,1)),normalize(vec3(-PHI-1,0,1)),normalize(vec3(1,PHI+1,0)),normalize(vec3(-1,PHI+1,0)),normalize(vec3(0,PHI,1)),normalize(vec3(0,-PHI,1)),normalize(vec3(1,0,PHI)),normalize(vec3(-1,0,PHI)),normalize(vec3(PHI,1,0)),normalize(vec3(-PHI,1,0)));"
 "const float material_ID0=1,material_ID1=2,material_ID2=3,material_ID3=4,material_ID4=5,material_ID5=6,material_ID6=7;"
 "uniform float _Objects[80];\n"
 "#ifdef DEBUG_STEPS\n"
 "float focus;\n"
 "#endif\n"
 "mat2 rot2(float th)"
 "{"
   "vec2 a=sin(vec2(1.5708,0)+th);"
   "return mat2(a,-a.y,a.x);"
 "}"
 "mat3 rotationMatrix(vec3 axis,float angle)"
 "{"
   "axis=normalize(axis);"
   "float s=sin(angle),c=cos(angle),oc=1.-c;"
   "return mat3(oc*axis.x*axis.x+c,oc*axis.x*axis.y-axis.z*s,oc*axis.z*axis.x+axis.y*s,oc*axis.x*axis.y+axis.z*s,oc*axis.y*axis.y+c,oc*axis.y*axis.z-axis.x*s,oc*axis.z*axis.x-axis.y*s,oc*axis.y*axis.z+axis.x*s,oc*axis.z*axis.z+c);"
 "}"
 "mat3 RotateQuaternion(vec4 q)"
 "{"
   "mat3 m;"
   "float a1,a2,s;"
   "s=q.w*q.w-.5;"
   "return mat3(q.x*q.x+s,q.x*q.y-q.z*q.w,q.x*q.z-q.y*q.w,q.x*q.y+q.z*q.w,q.y*q.y+s,q.y*q.z+q.x*q.w,q.x*q.z+q.y*q.w,q.y*q.z-q.x*q.w,q.z*q.z+s);"
 "}"
 "float Noise(vec2 p)"
 "{"
   "vec2 s=sin(p*.6345)+sin(p*1.62423);"
   "return dot(s,vec2(.125))+.5;"
 "}"
 "float hash11(float p)"
 "{"
   "vec3 p3=fract(vec3(p)*kHashScale1);"
   "p3+=dot(p3,p3.yyx+.5);"
   "return fract(p3.y*p3.z*p3.z*.2)-fract(p3.y*p3.z);"
 "}"
 "vec2 hash(vec2 p)"
 "{"
   "return p=vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))),-1.+2.*fract(sin(p)*43758.5);"
 "}"
 "float hash1(float h)"
 "{"
   "return fract(sin(h)*43758.5);"
 "}"
 "float rand(vec2 co)"
 "{"
   "return fract(sin(dot(co.xy,vec2(12.9898,78.233)))*43758.5);"
 "}"
 "float rand(float co)"
 "{"
   "return rand(vec2(co));"
 "}"
 "float perlinnoise(in vec2 p)"
 "{"
   "float K1=.366025,K2=.211325;"
   "vec2 i=floor(p+(p.x+p.y)*K1),a=p-i+(i.x+i.y)*K2,o=a.x>a.y?vec2(1.,0.):vec2(0.,1.),b=a-o+K2,c=a-1.+2.*K2;"
   "vec3 h=max(.5-vec3(dot(a,a),dot(b,b),dot(c,c)),0.),n=h*h*h*h*vec3(dot(a,hash(i)),dot(b,hash(i+o)),dot(c,hash(i+1.)));"
   "return dot(n,vec3(70.));"
 "}"
 "float hash12(vec2 p)"
 "{"
   "vec3 p3=fract(vec3(p.xyx)*.1031);"
   "p3+=dot(p3,p3.yzx+19.19);"
   "return fract((p3.x+p3.y)*p3.z);"
 "}"
 "vec3 filmgrain(vec3 color)"
 "{"
   "float noise=hash12(gl_FragCoord.xy+.001*_iTime)/3.+.85;"
   "return color*noise;"
 "}"
 "float GetHeightmap(vec3 p)"
 "{"
   "float hills,hill_1=8.*perlinnoise(.1*p.xz),hill_2=4.*perlinnoise(.5*p.xz),hill_3=2.*perlinnoise(p.xz),hill_4=perlinnoise(2.*p.xz);"
   "hills+=hill_1;"
   "hills+=hill_2;"
   "hills+=hill_3;"
   "hills+=hill_4;"
   "return hills;"
 "}"
 "float GetHeightmapLowPrecision(vec3 p)"
 "{"
   "float hills,hill_1=8.*perlinnoise(.1*p.xz);"
   "hills+=hill_1;"
   "return hills;"
 "}"
 "void pR(inout vec2 p,float a)"
 "{"
   "p=cos(a)*p+sin(a)*vec2(p.y,-p.x);"
 "}"
 "float celli(in vec3 p)"
 "{"
   "return p=fract(p)-.5,dot(p,p);"
 "}"
 "float cellTile(in vec3 p)"
 "{"
   "vec4 d;"
   "d.x=celli(p-vec3(.81,.62,.53));"
   "p.xy=vec2(p.y-p.x,p.y+p.x)*.7071;"
   "d.y=celli(p-vec3(.39,.2,.11));"
   "p.yz=vec2(p.z-p.y,p.z+p.y)*.7071;"
   "d.z=celli(p-vec3(.62,.24,.06));"
   "p.xz=vec2(p.z-p.x,p.z+p.x)*.7071;"
   "d.w=celli(p-vec3(.2,.82,.64));"
   "d.xy=min(d.xz,d.yw);"
   "return min(d.x,d.y)*2.66;"
 "}"
 "float smin(float a,float b,float k)"
 "{"
   "float res=exp(-k*a)+exp(-k*b);"
   "return-log(res)/k;"
 "}"
 "float length2(vec2 p)"
 "{"
   "return sqrt(p.x*p.x+p.y*p.y);"
 "}"
 "float length6(vec2 p)"
 "{"
   "return p=p*p*p,p=p*p,pow(p.x+p.y,1./6.);"
 "}"
 "float length8(vec2 p)"
 "{"
   "return p=p*p,p=p*p,p=p*p,pow(p.x+p.y,.125);"
 "}"
 "float sgn(float x)"
 "{"
   "return x<0?-1:1;"
 "}"
 "vec2 sgn(vec2 v)"
 "{"
   "return vec2(v.x<0?-1:1,v.y<0?-1:1);"
 "}"
 "float square(float x)"
 "{"
   "return x*x;"
 "}"
 "vec2 square(vec2 x)"
 "{"
   "return x*x;"
 "}"
 "vec3 square(vec3 x)"
 "{"
   "return x*x;"
 "}"
 "float lengthSqr(vec3 x)"
 "{"
   "return dot(x,x);"
 "}"
 "float maxMinus(float a,float b)"
 "{"
   "return max(-a,b);"
 "}"
 "float vmax(vec2 v)"
 "{"
   "return max(v.x,v.y);"
 "}"
 "float vmax(vec3 v)"
 "{"
   "return max(max(v.x,v.y),v.z);"
 "}"
 "float vmax(vec4 v)"
 "{"
   "return max(max(v.x,v.y),max(v.z,v.w));"
 "}"
 "float vmin(vec2 v)"
 "{"
   "return min(v.x,v.y);"
 "}"
 "float vmin(vec3 v)"
 "{"
   "return min(min(v.x,v.y),v.z);"
 "}"
 "float vmin(vec4 v)"
 "{"
   "return min(min(v.x,v.y),min(v.z,v.w));"
 "}"
 "float fGDF(vec3 p,float r,float e,int begin,int end)"
 "{"
   "float d=0;"
   "for(int i=begin;i<=end;++i)"
     "d+=pow(abs(dot(p,GDFVectors[i])),e);"
   "return pow(d,1/e)-r;"
 "}"
 "float fGDF(vec3 p,float r,int begin,int end)"
 "{"
   "float d=0;"
   "for(int i=begin;i<=end;++i)"
     "d=max(d,abs(dot(p,GDFVectors[i])));"
   "return d-r;"
 "}"
 "vec4 DistUnionCombine(in vec4 v1,in vec4 v2)"
 "{"
   "return mix(v1,v2,step(v2.x,v1.x));"
 "}"
 "vec4 DistUnionCombineTransparent(in vec4 v1,in vec4 v2,in float transparencyPointer)"
 "{"
   "vec4 vScaled=vec4(v2.x*(transparencyPointer*2.-1.),v2.yzw);"
   "return mix(v1,vScaled,step(vScaled.x,v1.x)*step(0.,transparencyPointer));"
 "}"
 "PointLight GetPointLight()"
 "{"
   "PointLight result;"
   "result.position=_PointLightPosition.xyz;"
   "result.color=_PointLightColor.xyz;"
   "result.position=-result.position;"
   "return result;"
 "}"
 "DirectionLight GetDirectionLight()"
 "{"
   "DirectionLight result;"
   "result.direction=_DirectionalLight.xyz;"
   "result.color=_DirectionalLightColor.xyz;"
   "result.direction=-result.direction;"
   "result.color*=.123;"
   "return result;"
 "}"
 "float pMod1(inout float p,float size)"
 "{"
   "float halfsize=size*.5,c=floor((p+halfsize)/size);"
   "p=mod(p+halfsize,size)-halfsize;"
   "return c;"
 "}"
 "void pR45(inout vec2 p)"
 "{"
   "p=(p+vec2(p.y,-p.x))*sqrt(.5);"
 "}"
 "float getTilt(vec3 p)"
 "{"
   "return dot(p,vec3(.299,.587,.114));"
 "}"
 "vec3 tex3D(sampler2D tex,in vec3 p,in vec3 n)"
 "{"
   "return n=max(n*n,.001),n/=n.x+n.y+n.z,(texture(tex,p.yz)*n.x+texture(tex,p.zx)*n.y+texture(tex,p.xy)*n.z).xyz;"
 "}"
 "vec3 doDisplacement(sampler2D tex,in vec3 p,in vec3 normal,float bumpPower)"
 "{"
   "float epsilon=.001;"
   "vec3 grad=vec3(getTilt(tex3D(tex,vec3(p.x-epsilon,p.y,p.z),normal)),getTilt(tex3D(tex,vec3(p.x,p.y-epsilon,p.z),normal)),getTilt(tex3D(tex,vec3(p.x,p.y,p.z-epsilon),normal)));"
   "grad=(grad-getTilt(tex3D(tex,p,normal)))/epsilon;"
   "grad-=normal*dot(normal,grad);"
   "return normalize(normal+grad*bumpPower);"
 "}"
 "float fBox(vec3 p,vec3 b)"
 "{"
   "vec3 d=abs(p)-b;"
   "return length(max(d,vec3(0)))+vmax(min(d,vec3(0)));"
 "}"
 "float Wheat(vec3 pos,vec3 algorithm)"
 "{"
   "pos.x+=cos(algorithm.z+pos.y*7.+pos.x*5.)*.03;"
   "pos.z-=.5;"
   "vec3 opos=pos;"
   "opos.x=mod(opos.x,algorithm.z)-algorithm.z/2.;"
   "opos.x+=mod(opos.z*1.5,algorithm.y)*algorithm.x;"
   "opos.z=mod(opos.z,algorithm.z)-algorithm.z/2.;"
   "opos.z-=mod(opos.z,algorithm.z/2.)*.1;"
   "float dist=fBox(opos+vec3(.2,0.,.2),vec3(.002,.2,.003));"
   "return dist;"
 "}"
 "float sdBox(vec3 p,vec3 b)"
 "{"
   "vec3 d=abs(p)-b;"
   "return length(max(d,0.))+min(max(d.x,max(d.y,d.z)),0.);"
 "}"
 "float Smoke(vec3 pos,vec3 algorithm)"
 "{"
   "vec3 opos=pos,opos_=opos;"
   "float i=0.,distance5=1.;"
   "return min(distance5,sdBox(opos+vec3(fract(-opos.z*.5+(.2+.5*cos(fract(opos.z*.2)*algorithm.x*.1)*.2)*opos.y)*sin(opos.y+i+opos.z*4.+algorithm.x)+1.,0.,i*.01),vec3((2.-opos.y)*.03*abs(cos(opos.y*2.+algorithm.x)),3.+fract(i),.9)));"
 "}"
 "float fOpUnionRund(float a,float b,float r)"
 "{"
   "vec2 u=max(vec2(r-a,r-b),vec2(0));"
   "return max(r,min(a,b))-length(u);"
 "}"
 "float TreeTrunk(vec3 pos,vec3 algorithm)"
 "{"
   "pos.y+=GetHeightmapLowPrecision(pos*algorithm)*algorithm.y;"
   "vec3 opos=pos,opos_=opos;"
   "float vali=5.;"
   "opos.x+=hash1(floor(pos.z/vali)+vali)*vali*2.;"
   "opos.x=mod(opos.x,vali)-vali/2.;"
   "opos.z+=hash1(floor(pos.x/vali)+vali)*vali*2.;"
   "opos.z=mod(opos.z,vali)-vali/2.;"
   "pR(opos.xz,pos.z);"
   "pR(opos.zy,3.14);"
   "float height=0.;"
   "opos.y+=.75;"
   "float dist=fBox(opos+vec3(0.,height,0.),vec3(.08+atan(opos.y*2.)*.02,1.5,.05));"
   "opos.y-=.75;"
   "vec3 o=opos+vec3(0.,height,0.);"
   "pR(opos.yx,5.);"
   "dist=fOpUnionRund(dist,fBox(opos+vec3(-.1+.1*cos(pos.z*10.1),0.,0.),vec3(.03,.5,.02)),.1);"
   "opos=o;"
   "return dist;"
 "}"
 "float fSphere(vec3 p,float r)"
 "{"
   "return length(p)-r;"
 "}"
 "float sdSphere(vec3 p,float s)"
 "{"
   "return length(p)-s;"
 "}"
 "float TreeBush(vec3 pos,vec3 algorithm)"
 "{"
   "pos.y+=GetHeightmapLowPrecision(pos*algorithm)*algorithm.y;"
   "vec3 opos=pos;"
   "float vali=5.;"
   "opos.x+=hash1(floor(pos.z/vali)+vali)*vali*2.;"
   "opos.x=mod(opos.x,vali)-vali/2.;"
   "opos.z+=hash1(floor(pos.x/vali)+vali)*vali*2.;"
   "opos.z=mod(opos.z,vali)-vali/2.;"
   "pR(opos.xz,pos.z);"
   "pR(opos.zy,3.14);"
   "float ss=.95+perlinnoise(floor(vec2(1.+opos.x*algorithm.x,.5+opos.z*algorithm.y)))*.1;"
   "vec3 o=opos;"
   "opos=o;"
   "opos.y+=GetHeightmapLowPrecision(pos*algorithm)*algorithm.y;"
   "float distance2=sdSphere(opos+vec3(.2*ss,2.*ss,0.),max(-(opos.y*.7*ss),0.));"
   "pos.y+=GetHeightmapLowPrecision(pos*algorithm)*algorithm.y;"
   "distance2=min(distance2,fBox(pos+vec3(0.,2.5,0.),vec3(1111.,2.,1111.)));"
   "distance2*=.2;"
   "distance2+=cellTile(pos)*.1;"
   "distance2+=cellTile(pos*12.)*.025;"
   "return distance2;"
 "}"
 "vec4 GetDistanceScene(vec3 position,in float transparencyPointer)"
 "{"
   "vec4 result=vec4(10000.,-1.,0.,0.);"
   "float id0_distance=1e+09;"
   "vec3 posID0=position-vec3(_Objects[0],_Objects[1],_Objects[2]);"
   "id0_distance=min(Wheat(posID0,vec3(_Objects[3],_Objects[4],_Objects[5])),id0_distance);"
   "vec3 posID3=position-vec3(_Objects[30],_Objects[31],_Objects[32]);"
   "id0_distance=min(Wheat(posID3,vec3(_Objects[33],_Objects[34],_Objects[35])),id0_distance);"
   "vec4 distID0=vec4(id0_distance,material_ID0,position.xz+vec2(position.y,0.));"
   "result=DistUnionCombineTransparent(result,distID0,transparencyPointer);"
   "float id1_distance=1e+09;"
   "vec3 posID1=position-vec3(_Objects[10],_Objects[11],_Objects[12]);"
   "posID1=RotateQuaternion(vec4(_Objects[16],_Objects[17],_Objects[18],-_Objects[19]))*posID1;"
   "id1_distance=min(fBox(posID1,vec3(_Objects[13],_Objects[14],_Objects[15])),id1_distance);"
   "vec4 distID1=vec4(id1_distance,material_ID1,position.xz+vec2(position.y,0.));"
   "result=DistUnionCombine(result,distID1);"
   "float id2_distance=1e+09;"
   "vec3 posID2=position-vec3(_Objects[20],_Objects[21],_Objects[22]);"
   "id2_distance=min(Smoke(posID2,vec3(_Objects[23],_Objects[24],_Objects[25])),id2_distance);"
   "vec4 distID2=vec4(id2_distance,material_ID2,position.xz+vec2(position.y,0.));"
   "result=DistUnionCombine(result,distID2);"
   "float id3_distance=1e+09;"
   "\n#define posID4 position\n"
   "id3_distance=min(TreeTrunk(posID4,vec3(_Objects[43],_Objects[44],_Objects[45])),id3_distance);"
   "vec4 distID3=vec4(id3_distance,material_ID3,position.xz+vec2(position.y,0.));"
   "result=DistUnionCombine(result,distID3);"
   "float id4_distance=1e+09;"
   "vec3 posID5=position-vec3(_Objects[50],_Objects[51],_Objects[52]);"
   "id4_distance=min(fSphere(posID5,_Objects[53]),id4_distance);"
   "vec4 distID4=vec4(id4_distance,material_ID4,position.xz+vec2(position.y,0.));"
   "result=DistUnionCombine(result,distID4);"
   "float id5_distance=1e+09;"
   "vec3 posID6=position-vec3(_Objects[60],_Objects[61],_Objects[62]);"
   "id5_distance=min(fBox(posID6,vec3(_Objects[63],_Objects[64],_Objects[65])),id5_distance);"
   "vec4 distID5=vec4(id5_distance,material_ID5,position.xz+vec2(position.y,0.));"
   "result=DistUnionCombine(result,distID5);"
   "float id6_distance=1e+09;"
   "\n#define posID7 position\n"
   "id6_distance=min(TreeBush(posID7,vec3(_Objects[73],_Objects[74],_Objects[75])),id6_distance);"
   "vec4 distID6=vec4(id6_distance,material_ID6,position.xz+vec2(position.y,0.));"
   "result=DistUnionCombine(result,distID6);"
   "return result;"
 "}"
 "Material RockPattern(vec3 position)"
 "{"
   "vec3 _position=position;"
   "_position.y+=abs(_position.z)/12.;"
   "vec3 _position2=_position,_position3=position,_position4=position;"
   "float dist=1e+08,dist2=1e+08,heightmap=perlinnoise(_position.xz/6.);"
   "heightmap+=perlinnoise(_position.xz*12.)/64.;"
   "heightmap+=perlinnoise(_position.xz*40.)/128.;"
   "dist+=(perlinnoise(_position.xy/3.)*sin(_position.z)+perlinnoise(_position.xy)*cos(_position.z))/3.;"
   "float heightmap2=perlinnoise(_position.xz*2.)/6.,a=heightmap;"
   "vec3 color=vec3(1.)-6.*vec3(.4,.6,.8)*perlinnoise(_position.xz/6.)*perlinnoise(_position.xz*12.);"
   "dist-=(perlinnoise(_position.xz*120.)+perlinnoise(_position.zy*120.))/880.;"
   "vec3 kummaj=vec3(.8,1.,.4);"
   "color+=kummaj/(1.+.6*pow(length(_position-vec3(1.,0.,4.)),2.));"
   "Material mat;"
   "mat.albedo=color;"
   "mat.reflectionCoefficient=.1;"
   "mat.smoothness=.1;"
   "mat.transparency=0.;"
   "mat.reflectindx=.1;"
   "return mat;"
 "}"
 "Material GetObjectMaterial(in ContactInfo hitNfo)"
 "{"
   "Material mat;"
   "if(hitNfo.id.x==material_ID0)"
     "mat.reflectionCoefficient=.1,mat.albedo=vec3(.951649,.990566,.0233624),mat.transparency=.258824,mat.smoothness=.1,mat.reflectindx=.2;"
   "if(hitNfo.id.x==material_ID1)"
     "mat.reflectionCoefficient=.178,mat.albedo=vec3(1,1,1),mat.transparency=0,mat.smoothness=.37,mat.reflectindx=.54;"
   "if(hitNfo.id.x==material_ID2)"
     "mat.reflectionCoefficient=.71,mat.albedo=vec3(1,0,0),mat.transparency=0,mat.smoothness=.84,mat.reflectindx=.8;"
   "if(hitNfo.id.x==material_ID3)"
     "mat.reflectionCoefficient=.04,mat.albedo=vec3(.650943,.377296,.267133),mat.transparency=0,mat.smoothness=.51,mat.reflectindx=.03;"
   "if(hitNfo.id.x==material_ID4)"
     "mat.reflectionCoefficient=.27,mat.albedo=vec3(1,1,1),mat.transparency=0,mat.smoothness=.8,mat.reflectindx=.69;"
   "if(hitNfo.id.x==material_ID5)"
     "mat=RockPattern(hitNfo.position);"
   "if(hitNfo.id.x==material_ID6)"
     "mat.reflectionCoefficient=.11,mat.albedo=vec3(.39781,.660377,.289694),mat.transparency=0,mat.smoothness=.17,mat.reflectindx=.05;"
   "return mat;"
 "}"
 "vec3 GetSkyGradient(vec3 rayDirection)"
 "{"
   "vec3 lightDir=GetDirectionLight().direction;"
   "lightDir=normalize(lightDir);"
   "float directLight=max(dot(rayDirection,lightDir),0.);"
   "vec3 backdrop=min(max(pow(directLight,41.)*vec3(1.8,1.1,.9)*.6,.01),1.);"
   "backdrop+=min(max(pow(directLight,2.5)*vec3(.8,.9,1.)*1.6,.01),1.);"
   "backdrop+=min(max(pow(directLight,81.)*1.6,.01),1.)*1.1;"
   "return max(backdrop,vec3(.8,.9,1.)*.125);"
 "}"
 "void ApplyAtmosphere(inout vec3 col,const in Trace ray,const in ContactInfo hitInfo)"
 "{"
   "float fogAmount=exp(hitInfo.distanc*-fogDensity*.1);"
   "vec3 fog=GetSkyGradient(ray.direction);"
   "DirectionLight directionalLight=GetDirectionLight();"
   "float dirDot=clamp(dot(-directionalLight.direction,ray.direction),0.,1.);"
   "fog+=directionalLight.color*pow(dirDot,10.);"
   "col=mix(fog,col,fogAmount);"
   "PointLight pointLight=GetPointLight();"
   "vec3 vToLight=pointLight.position-ray.origin;"
   "float pointDot=dot(vToLight,ray.direction);"
   "pointDot=clamp(pointDot,0.,hitInfo.distanc);"
   "vec3 closestPoint=ray.origin+ray.direction*pointDot;"
   "float fDist=length(closestPoint-pointLight.position);"
   "col+=pointLight.color*.01/(fDist*fDist);"
 "}"
 "vec3 GetNormal(in vec3 position,in float transparencyPointer)"
 "{"
   "float delta=.025;"
   "vec3 offset[4];"
   "offset[0]=vec3(delta,-delta,-delta);"
   "offset[1]=vec3(-delta,-delta,delta);"
   "offset[2]=vec3(-delta,delta,-delta);"
   "offset[3]=vec3(delta,delta,delta);"
   "float f1=GetDistanceScene(position+offset[0],transparencyPointer).x,f2=GetDistanceScene(position+offset[1],transparencyPointer).x,f3=GetDistanceScene(position+offset[2],transparencyPointer).x,f4=GetDistanceScene(position+offset[3],transparencyPointer).x;"
   "vec3 normal=normalize(offset[0]*f1+offset[1]*f2+offset[2]*f3+offset[3]*f4);"
   "return normal;"
 "}"
 "void RayMarch(in Trace ray,out ContactInfo result,int maxIter,float transparencyPointer)"
 "{"
   "ContactInfo originalResult=result;"
   "result.distanc=ray.startdistanc;"
   "result.id.x=0.;"
   "for(int i=0;i<=maxIter;i++)"
     "{"
       "result.position=ray.origin+ray.direction*result.distanc;"
       "vec4 sceneDistance=GetDistanceScene(result.position,transparencyPointer);"
       "float cocs=max(result.distanc-_Distance,0.)*_LensCoeff*.1;"
       "cocs=min(cocs,_MaxCoC*.1);"
       "result.id=sceneDistance.yzw;"
       "result.distanc=result.distanc+sceneDistance.x*_Step;"
       "if(sceneDistance.x<max(cocs,_MarchMinimum*.1)||result.distanc>_FarPlane)"
         "{"
           "sceneDistance=GetDistanceScene(result.position,transparencyPointer);"
           "\n#ifdef DEBUG_STEPS\n"
           "focus=cocs;"
           "\n#endif\n"
           "break;"
         "}"
     "}"
   "if(result.distanc>=ray.length)"
     "result.distanc=1000.,result.position=ray.origin+ray.direction*result.distanc,result.id.x=0.;"
 "}"
 "void insideMarch(in Trace ray,out ContactInfo result,int maxIter,float transparencyPointer)"
 "{"
   "result.distanc=ray.startdistanc;"
   "result.id.x=0.;"
   "for(int i=0;i<=maxIter/3;i++)"
     "{"
       "result.position=ray.origin+ray.direction*result.distanc;"
       "vec4 sceneDistance=GetDistanceScene(result.position,transparencyPointer);"
       "result.id=sceneDistance.yzw;"
       "result.distanc=result.distanc+sceneDistance.x;"
       "if(sceneDistance.y!=material_ID2)"
         "return;"
     "}"
   "if(result.distanc>=ray.length)"
     "result.distanc=1000.,result.position=ray.origin+ray.direction*result.distanc,result.id.x=0.;"
 "}"
 "float traceToLight(vec3 rayPosition,vec3 normalTrace,vec3 lightDir,float rayLightDistance)"
 "{"
   "vec3 ro=rayPosition,rd=lightDir;"
   "float t=.1,k=rayLightDistance,res=1.;"
   "for(int i=0;i<20;i++)"
     "{"
       "float h=GetDistanceScene(ro+rd*t,transparencyInformation).x;"
       "h=max(h,0.);"
       "res=min(res,k*h/t);"
       "t+=clamp(h,.001,.9);"
     "}"
   "return clamp(res,.1,9.);"
 "}"
 "float GetShadow(in vec3 position,in vec3 normal,in vec3 lightDirection,in float lightDistance)"
 "{"
   "return traceToLight(position,normal,lightDirection,lightDistance);"
 "}"
 "float GetAmbientOcclusion(in ContactInfo intersection,in Surface surface)"
 "{"
   "vec3 position=intersection.position,normal=surface.normal;"
   "float AO=1.,sdfDistance=0.;"
   "for(int i=0;i<=5;i++)"
     "{"
       "sdfDistance+=.1;"
       "vec4 sceneDistance=GetDistanceScene(position+normal*sdfDistance,transparencyInformation);"
       "AO*=1.-max(0.,(sdfDistance-sceneDistance.x)*.4/sdfDistance);"
     "}"
   "return AO;"
 "}"
 "void AddAtmosphere(inout vec3 col,in Trace ray,in ContactInfo hitNfo)"
 "{"
   "float fFogAmount=exp(hitNfo.distanc*-fogDensity);"
   "vec3 fogColor=GetSkyGradient(ray.direction);"
   "DirectionLight directionalLight=GetDirectionLight();"
   "float fDirDot=clamp(dot(-directionalLight.direction,ray.direction),0.,1.);"
   "fogColor+=directionalLight.color*pow(fDirDot,20.);"
   "PointLight pointLight=GetPointLight();"
   "vec3 lightPointer=pointLight.position-ray.origin;"
   "float PNT_dot=dot(lightPointer,ray.direction);"
   "PNT_dot=clamp(PNT_dot,0.,hitNfo.distanc);"
   "vec3 closestPoint=ray.origin+ray.direction*PNT_dot;"
   "float sdfDistance=length(closestPoint-pointLight.position);"
   "col+=pointLight.color*.01/(sdfDistance*sdfDistance);"
 "}"
 "vec3 AddFresnel(in vec3 diffuse,in vec3 specular,in vec3 normal,in vec3 viewDirection,in Material material)"
 "{"
   "vec3 reflection=reflect(viewDirection,normal),reflectionToView=normalize(reflection+-viewDirection);"
   "float reflectionCoefficient=material.reflectionCoefficient,smoothFactor=material.smoothness*.9+.1,r1=dot(reflectionToView,-viewDirection);"
   "r1=clamp(1.-r1,0.,1.);"
   "float r1Pow=pow(r1,5.),fresnelApprox=reflectionCoefficient+(1.-reflectionCoefficient)*r1Pow*smoothFactor;"
   "return mix(diffuse,specular,fresnelApprox);"
 "}"
 "float BlinnPhong(in vec3 collisionDirection,in vec3 lightDirection,in vec3 normal,in float smoothness)"
 "{"
   "vec3 reflectionToView=normalize(lightDirection-collisionDirection);"
   "float n_dot_h=max(0.,dot(reflectionToView,normal)),specularPower=exp2(4.+6.*smoothness),specularIntensity=(specularPower+2.)*.125;"
   "return pow(n_dot_h,specularPower)*specularIntensity;"
 "}"
 "Shading AddPointLight(in PointLight light,in vec3 surfacePosition,in vec3 collisionDirection,in vec3 normal,in Material material)"
 "{"
   "Shading shading;"
   "vec3 lightPointer=light.position-surfacePosition,lightDirection=normalize(lightPointer);"
   "float shadowLength=length(lightPointer),attenuation=1./(shadowLength*shadowLength),shadowFactor=GetShadow(surfacePosition,normal,lightDirection,shadowLength);"
   "vec3 diffuse=light.color*max(0.,shadowFactor*attenuation*dot(lightDirection,normal)/(1.+material.transparency));"
   "shading.diffuse=diffuse;"
   "shading.specular=BlinnPhong(collisionDirection,lightDirection,normal,material.smoothness)*diffuse;"
   "shading.diffuse=clamp(shading.diffuse,0.,1.);"
   "shading.specular=clamp(shading.specular,0.,1.);"
   "return shading;"
 "}"
 "Shading AddDirectionLight(in DirectionLight light,in vec3 surfacePosition,in vec3 collisionDirection,in vec3 normal,in Material material)"
 "{"
   "Shading shading;"
   "float shadowLength=10.;"
   "vec3 lightDirection=-light.direction;"
   "float shadowFactor=GetShadow(surfacePosition,normal,lightDirection,shadowLength);"
   "vec3 diffuse=light.color*shadowFactor*max(0.,dot(lightDirection,normal)/(1.+material.transparency));"
   "shading.diffuse=diffuse;"
   "shading.specular=BlinnPhong(collisionDirection,lightDirection,normal,material.smoothness)*diffuse;"
   "shading.diffuse=clamp(shading.diffuse,0.,1.);"
   "shading.specular=clamp(shading.specular,0.,1.);"
   "return shading;"
 "}"
 "vec3 ShadeSurface(in Trace ray,in ContactInfo hitNfo,in Surface surface,in Material material)"
 "{"
   "Shading shading;"
   "float AO=GetAmbientOcclusion(hitNfo,surface);"
   "vec3 ambientLight=GetSkyGradient(surface.normal)*AO;"
   "shading.diffuse=ambientLight;"
   "shading.specular=surface.reflection;"
   "PointLight pointLight=GetPointLight();"
   "Shading pointLighting=AddPointLight(pointLight,hitNfo.position,ray.direction,surface.normal,material);"
   "shading.diffuse+=pointLighting.diffuse;"
   "shading.specular+=pointLighting.specular;"
   "DirectionLight directionalLight=GetDirectionLight();"
   "Shading directionLighting=AddDirectionLight(directionalLight,hitNfo.position,ray.direction,surface.normal,material);"
   "shading.diffuse+=directionLighting.diffuse;"
   "shading.specular+=directionLighting.specular;"
   "vec3 diffuseReflection=mix(shading.diffuse*material.albedo*(1.-material.transparency),surface.subsurface,material.transparency);"
   "return AddFresnel(diffuseReflection,shading.specular,surface.normal,ray.direction,material);"
 "}"
 "vec3 GetSceneColourSecondary(in Trace ray)"
 "{"
   "ContactInfo hitNfo;"
   "RayMarch(ray,hitNfo,22,noTransparency);"
   "vec3 sceneColor;"
   "if(hitNfo.id.x<.5)"
     "sceneColor=GetSkyGradient(ray.direction);"
   "else"
     "{"
       "Surface surface;"
       "surface.normal=GetNormal(hitNfo.position,noTransparency);"
       "Material material=GetObjectMaterial(hitNfo);"
       "surface.reflection=GetSkyGradient(reflect(ray.direction,surface.normal));"
       "material.transparency=0.;"
       "sceneColor=ShadeSurface(ray,hitNfo,surface,material);"
     "}"
   "AddAtmosphere(sceneColor,ray,hitNfo);"
   "return sceneColor;"
 "}"
 "vec3 GetReflection(in Trace ray,in ContactInfo hitNfo,in Surface surface)"
 "{"
   "const float lightOffSurface=.1;"
   "Trace reflectTrace;"
   "reflectTrace.direction=reflect(ray.direction,surface.normal);"
   "reflectTrace.origin=hitNfo.position;"
   "reflectTrace.length=16.;"
   "reflectTrace.startdistanc=lightOffSurface/abs(dot(reflectTrace.direction,surface.normal));"
   "return GetSceneColourSecondary(reflectTrace);"
 "}"
 "vec3 GetSubSurface(in Trace ray,in ContactInfo hitNfo,in Surface surface,in Material material)"
 "{"
   "float lightOffSurface=.05;"
   "Trace refractTrace;"
   "refractTrace.direction=refract(ray.direction,surface.normal,material.reflectindx);"
   "refractTrace.origin=hitNfo.position;"
   "refractTrace.length=16.;"
   "refractTrace.startdistanc=lightOffSurface/abs(dot(refractTrace.direction,surface.normal));"
   "ContactInfo hitNfo2;"
   "insideMarch(refractTrace,hitNfo2,32,emptyInformation);"
   "vec3 normal=GetNormal(hitNfo2.position,emptyInformation);"
   "Trace refractTrace2;"
   "refractTrace2.direction=refract(refractTrace.direction,normal,1./material.reflectindx);"
   "refractTrace2.origin=hitNfo2.position;"
   "refractTrace2.length=16.;"
   "refractTrace2.startdistanc=0.;"
   "float extinctionDistance=hitNfo2.distanc,transparencity=material.transparency/(1.+distance(hitNfo2.position,hitNfo.position)*4.),nonTransparency=1.-transparencity;"
   "vec3 sceneColor=material.albedo*GetSceneColourSecondary(refractTrace2)*nonTransparency,materialExtinction=material.albedo,extinction=1.7/(1.+materialExtinction*extinctionDistance);"
   "return sceneColor*extinction;"
 "}"
 "vec3 Reinhard(in vec3 color)"
 "{"
   "return vec3(1.,1.,1.)-exp2(-color);"
 "}"
 "vec4 mainImage()"
 "{"
   "vec2 fragCoord=gl_FragCoord.xy;"
   "vec4 fragColor;"
   "Trace ray;"
   "vec2 uv=fragCoord.xy/_iResolution.xy;"
   "if(_TextId>=1.)"
     "{"
       "vec3 text=texture(_TextTex,uv).xyz;"
       "fragColor.xyz=text.xyz;"
       "return fragColor;"
     "}"
   "vec3 lookAt=_CameraLookAt.xyz,position=_CameraPosition.xyz;"
   "float fov=_FOV;"
   "vec3 forwardDirection=normalize(lookAt-position),worldUpDirection=_CameraUp.xyz;"
   "vec2 viewDirectionCoord=uv*2.-1.;"
   "float aspectRatio=_iResolution.x/_iResolution.y;"
   "viewDirectionCoord.y/=aspectRatio;"
   "ray.origin=position;"
   "vec3 rightDirection=normalize(cross(forwardDirection,worldUpDirection)),upDirection=cross(rightDirection,forwardDirection);"
   "forwardDirection*=tan((90.-fov*.5)*DEG_TO_RAD);"
   "ray.direction=normalize(-rightDirection*viewDirectionCoord.x+upDirection*viewDirectionCoord.y+forwardDirection);"
   "ray.startdistanc=0.;"
   "ray.length=farClip;"
   "ray.direction.y*=.85;"
   "ContactInfo intersection;"
   "RayMarch(ray,intersection,128,transparencyInformation);"
   "vec3 sceneColor;"
   "float d=intersection.distanc;"
   "if(intersection.id.x<.5)"
     "sceneColor=GetSkyGradient(ray.direction);"
   "else"
     "{"
       "Surface surface;"
       "surface.normal=GetNormal(intersection.position,transparencyInformation);"
       "Material material=GetObjectMaterial(intersection);"
       "float distanctrans=intersection.distanc;"
       "sceneColor=ShadeSurface(ray,intersection,surface,material);"
     "}"
   "ApplyAtmosphere(sceneColor,ray,intersection);"
   "float exposure=1.5;"
   "vec2 coord=(uv-.5)*(_iResolution.x/_iResolution.y)*2.;"
   "float falloff=.2,rf=sqrt(dot(coord,coord))*falloff,rf2_1=rf*rf+1.,e=1.2/(rf2_1*rf2_1);"
   "vec3 noise=(rand(uv+_iTime)-.5)*vec3(1.,1.,1.)*.01;"
   "fragColor=min(max(vec4(e*Reinhard(sceneColor*exposure)+noise,1.),vec4(0.,0.,0.,1.)),vec4(1.,1.,1.,1.));"
   "fragColor.xyz=fragColor.xyz+filmgrain(fragColor.xyz)*.2;"
   "float cocs=(d-_Distance)*_LensCoeff/d;"
   "cocs=clamp(cocs,-_MaxCoC,_MaxCoC);"
   "fragColor.w=saturate(abs(cocs)*_RcpMaxCoC);"
   "\n#ifdef DEBUG_STEPS\n"
   "fragColor.x=focus;"
   "\n#endif\n"
   "return fragColor;"
 "}"
 "void main()"
 "{"
   "gl_FragColor=mainImage();"
 "}";

#endif // FRAGMENT_INL_
