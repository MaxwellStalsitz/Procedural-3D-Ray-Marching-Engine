#version 430 core
out vec4 FragColor;

uniform vec2 resolution;
uniform float time;
uniform float deltaTime;
uniform vec2 mousePosition;
uniform bool useLighting;

uniform vec3 direction;
uniform vec3 cameraPos;
uniform vec3 cameraFront;

const float FOV = 1.0;
uniform int MAX_STEPS;
uniform float MAX_DIST;
uniform float MIN_DIST;

uniform bool antiAliasing;

uniform bool animate;
uniform float timeMultiplier;

uniform int scene;

uniform int numberOfObjects;
uniform vec3 objectPositions[25];
uniform vec3 objectScale[25];

uniform float power;
uniform int iterations;

uniform bool ambientOcclusion;
uniform int samples;

uniform bool reflections;
uniform float reflectionVisibility;

uniform bool fogEnabled;
uniform float fogVisibility;

#define PI 3.1415926535897932384626433832795

//----------------------------------------------------------------------
vec3 background;
//----------------------------------------------------------------------

vec2 mergeResults(vec2 res1, vec2 res2) 
{
    return (res1.x < res2.x) ? res1 : res2;
}

//from inigo quilez - https://iquilezles.org/articles/smin
vec2 smoothMinimum(vec2 a, vec2 b, float k) {
  float h = max(k - abs(a.x - b.x), 0.0) / k;
  float id = (b.x < b.x) ? a.y : b.y;
  return vec2(min(a.x, b.x) - h * h * h * k * 1.0 / 6.0, id);
}

vec2 differenceInResults(vec2 res1, vec2 res2)
{
	return (res1.x > -res2.x) ? res1 : vec2(-res2.x, res2.y);
}

//----------------------------------------------------------------------
//distance functions (from inigo quilez)

float sdBox( vec3 p, vec3 b )
{
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float sdBoxFrame( vec3 p, vec3 b, float e )
{
      p = abs(p  )-b;
	  vec3 q = abs(p+e)-e;
	  return min(min(
      length(max(vec3(p.x,q.y,q.z),0.0))+min(max(p.x,max(q.y,q.z)),0.0),
      length(max(vec3(q.x,p.y,q.z),0.0))+min(max(q.x,max(p.y,q.z)),0.0)),
      length(max(vec3(q.x,q.y,p.z),0.0))+min(max(q.x,max(q.y,p.z)),0.0));
}

float sdSphere(vec3 p, float s)
{
  return length(p)-s;
}

float sdRoundBox( vec3 p, vec3 b, float r )
{
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0) - r;
}

float sdTorus( vec3 p, vec2 t )
{
  vec2 q = vec2(length(p.xz)-t.x,p.y);
  return length(q)-t.y;
}

float sdOctahedron( vec3 p, float s)
{
  p = abs(p);
  float m = p.x+p.y+p.z-s;
  vec3 q;
       if( 3.0*p.x < m ) q = p.xyz;
  else if( 3.0*p.y < m ) q = p.yzx;
  else if( 3.0*p.z < m ) q = p.zxy;
  else return m*0.57735027;
    
  float k = clamp(0.5*(q.z-q.y+s),0.0,s); 
  return length(vec3(q.x,q.y-s+k,q.z-k)); 
}

//----------------------------------------------------------------------
//map functions

vec2 customScene(vec3 rayPosition){
	float distanceToSDF;
	vec2 result;

	//id for material
	float ID;

	//infinite plane
	ID = 2.0;
	float planeHeight = 1.0;
	float planeDistance = rayPosition.y + planeHeight;
	distanceToSDF = planeDistance;
	vec2 plane = vec2(distanceToSDF, ID);
	result = plane;

	for (int i = 0; i < numberOfObjects; i++){
		ID = 1.0;
		vec3 position = objectPositions[i];
		float sphereRadius = 1;
		distanceToSDF = sdSphere(rayPosition-position, sphereRadius);
		vec2 sphere1 = vec2(distanceToSDF, ID);

		result = mergeResults(result, sphere1);
	}
	
	return result;
}

vec3 rotateX(vec3 p, float a) {
    return vec3(p.x, cos(a) * p.y - sin(a) * p.z, sin(a) * p.y + cos(a) * p.z);
}
    
vec3 rotateY(vec3 p, float a) {
    return vec3(cos(a) * p.x + sin(a) * p.z, p.y, -sin(a) * p.x + cos(a) * p.z);
}

float sdHouse( vec3 p )
{
    vec3 q = abs(p) - vec3(1.0, 1.0, 0.5);
    return min(max(q.x,max(q.y,q.z)),0.0) + length(max(q,0.0));
}

vec2 demoScene(vec3 pos)
{
	float distanceToSDF;

	float ID = 3.0;
	//vec3 position = vec3(-.5,0,-6);
	vec3 position = vec3(0,0,-3);
	//distanceToSDF = sdTorus(rotateX(pos-position, 90.0 * PI / 180.0), vec2(0.75,0.15));
	distanceToSDF = sdSphere(pos - position, 1);
	vec2 sphere1 = vec2(distanceToSDF, ID);

	ID = 5.0;
	position = vec3(2, 0, -3);
	distanceToSDF = sdOctahedron(pos-position, 1);
	vec2 sphere2 = vec2(distanceToSDF, ID);

	ID = 3.0;
	position = vec3(-3.0, -0.25, -3.0);
	distanceToSDF = sdBoxFrame(pos-position, vec3(0.75, 0.75, 0.75), 0.05);
	vec2 box = vec2(distanceToSDF, ID);
	
	ID = 2.0;
	float planeDistance = pos.y + 1.0;
	distanceToSDF = planeDistance;
	vec2 plane = vec2(distanceToSDF, ID);

	vec2 result;
	result = mergeResults(plane, sphere1);
	//result = mergeResults(result, sphere2);
	//result = mergeResults(result, box);

	return result;
}

vec2 manyEntityScene(vec3 pos){
	
	vec2 result;

	float ID, distanceToSDF;

	ID = 2.0;
	float planeHeight = 1.0;
	float planeDistance = pos.y + planeHeight;
	distanceToSDF = planeDistance;
	vec2 plane = vec2(distanceToSDF, ID);
	result = plane;

	for (int i = 0; i < 100; i++){
		
		// make a 10x10 row of spheres
		float x = float(i % 10) * 2.5;
		float z = float(i / 10) * 2.5;
		vec3 position = vec3(x, 0.0, z);

		distanceToSDF = sdSphere(pos-position, 1);
		vec2 sphere = vec2(distanceToSDF, 1.0);

		result = mergeResults(result, sphere);
	}

	return result;
}

vec2 infiniteScene(vec3 pos){
	pos = mod(pos, 4.0) - 4.0 * 0.5;

	float id = 6.0;

	vec2 sphere1 = vec2(length(pos) - 0.5, id);

	return sphere1;
}

float plane( vec3 p, vec4 n ) {
  return dot(p,n.xyz) + n.w;
}

#define BACK_WALL_SDF plane(pos, vec4(0.0, 0.0, -1.0, 6.0))
#define FRONT_WALL_SDF plane(pos + vec3(0.0,0.0,25.0), vec4(0.0, 0.0, -1.0, 6.0))
#define LEFT_WALL_SDF plane(pos, vec4(1.0, 0.0, 0.0, 4.0))
#define RIGHT_WALL_SDF plane(pos, vec4(-1.0, 0.0, 0.0, 4.0))
#define CEILING_SDF plane(pos, vec4(0.0, -1.0, 0.0, 5))
#define FLOOR_SDF plane(pos, vec4(0.0, 1.0, 0.0, 2.0))
#define TALL_BOX_SDF sdBox(rotateY(pos + vec3(-1.5, 0.0, -2.0), 65.0 * PI / 180.0), vec3(1.0, 2.0, 1.2))
#define SMALL_BOX_SDF sdBox(rotateY(pos + vec3(1.5, 1.0, 0.0), -65.0 * PI / 180.0), vec3(1.0, 1.0, 1.0))

vec2 cornellBox(vec3 pos){
    vec2 t = vec2(BACK_WALL_SDF, 1.0);
    
    float t2;
    if((t2 = LEFT_WALL_SDF) < t.x) {
        t = vec2(t2, 4.0);
    }
    if((t2 = RIGHT_WALL_SDF) < t.x) {
        t = vec2(t2, 3.0);
    }
    if((t2 = CEILING_SDF) < t.x) {
        t = vec2(t2, 1.0);
    }
    if((t2 = FLOOR_SDF) < t.x) {
        t = vec2(t2, 1.0);
    }  

	float tallBox = sdBox(rotateY(pos + vec3(-1.5, 0.0, -2.0), 65.0 * PI / 180.0), vec3(1.0, 2.0, 1.2));
	vec2 tallSDF = vec2(tallBox, 1.0);
	float smallBox = sdBox(rotateY(pos + vec3(1.5, 1.0, 0.0), -65.0 * PI / 180.0), vec3(1.0, 1.0, 1.0));
	vec2 smallSDF = vec2(smallBox, 1.0);
	
	t = mergeResults(t, tallSDF);
	t = mergeResults(t, smallSDF);

	return t;
}

vec2 mandelbulb(vec3 pos) 
{
	vec3 z = pos;
	vec3 dz=vec3(0.0);
	float r, theta, phi;
	float dr = 1.0;
	
	float t0 = 1.0;
	for(int i = 0; i < iterations; ++i) {
		r = length(z);
		if(r > 2.0) continue;
		theta = atan(z.y / z.x);
        if (animate)
			phi = asin(z.z / r) + time*timeMultiplier;
        else
			phi = asin(z.z / r);
		
		dr = pow(r, power - 1.0) * dr * power + 1.0;
	
		r = pow(r, power);
		theta = theta * power;
		phi = phi * power;
		
		z = r * vec3(cos(theta)*cos(phi), sin(theta)*cos(phi), sin(phi)) + pos;
		
		t0 = min(t0, r);
	}
	return vec2(0.5 * log(r) * r / dr, 1.0);
}

//choosing map function based on scene
vec2 map(vec3 rayPosition)
{
	vec2 result;

	if (scene == 1)
		result = demoScene(rayPosition);
	else if (scene == 2)
		result = mandelbulb(rayPosition);
	else if (scene == 3)
		result = customScene(rayPosition);
	else if (scene == 4)
		result = cornellBox(rayPosition);
	
	return result;
}

//----------------------------------------------------------------------

int colorIteration = 0;

//getting material from material id
vec3 getMaterial(vec3 p, float id) 
{
    vec3 m;

	if (id == 0.0)
		id = 0.0; //custom material
	else if (id == 1.0)
		m = vec3(1.0, 1.0, 1.0);
	else if (id == 2.0)
		m = mix(vec3(0.0 + 1.0 * mod(floor(p.x) + floor(p.z), 2.0)), vec3(0.773, 0.725, 0.627), 0.5); //from inigo quilez
	else if (id == 3.0)
		m = vec3(0.61, 0.176, 0.176);
	else if (id == 4.0)
		m = vec3(0.49, 0.74, 0.4);
	else if (id == 5.0)
		m = vec3(0.247, 0.427, 0.819);
	else if (id == 6.0){ // random
	vec3 ip = floor(p);


	float rnd = fract(sin(dot(ip, vec3(27.17, 112.61, 57.53)))*43758.5453);
    
    // Color up the objects in a cubic checkered arrangement using a subtle version
    // of IQ's palette formula.
    m = (fract(dot(ip, vec3(.5))) > .001)? 
         .5 + .45*cos(mix(3., 4., rnd) + vec3(.9, .45, 1.5)) //vec3(.6, .3, 1.)
         : vec3(.7 + .3*rnd);
    
	}

	
    return m;
}

//calculating ambient occlusion
float getOcclusion(vec3 pos, vec3 normal)
{
	float occ = 0.0;
	float weight = 1.0;
	for (int i = 0; i < samples; i++){
		float len = 0.01 + 0.02 * float(i*i);
		float dist = map(pos+normal*len).x; //closest point in the scene
		occ += (len-dist)*weight;
		weight *= 0.85;
	}
	return 1.0 - clamp(0.6 * occ, 0.0, 1.0);
}

//camera function
mat3 getCam(vec3 rayOrigin)
{
	vec3 camR = normalize(cross(vec3(0,1,0),cameraFront));
	vec3 camU = cross(cameraFront,camR);
	return mat3(-camR, camU, cameraFront);
}

//calculating normals
vec3 calcNormal(vec3 position)
{
	vec2 e = vec2(1.0, -1.0) * 0.0005;
	return normalize(
		e.xyy * map(position + e.xyy).x +
		e.yyx * map(position + e.yyx).x +
		e.yxy * map(position + e.yxy).x +
		e.xxx * map(position + e.xxx).x);
}

//soft shadow function (https://iquilezles.org/articles/rmshadows/)
float softshadow(vec3 ro, vec3 rd, float mint, float maxt, float k)
{
    float res = 1.0;
	float dist = mint;
    for(int i = 0; i < MAX_STEPS; i++){
        float h = map(ro + rd* dist).x;

        if( h< MIN_DIST )
            return 0.0;

        res = min( res, k*h/dist );

		if (h > MAX_DIST)
			break;
			
        dist += h;
    }
    return res;
}

//calculating uv
vec2 getUV(vec2 offset){
	return ((gl_FragCoord.xy + offset) - 0.5 * resolution.xy) / resolution.y;
}

//ray marching pass for calculating standard and reflection values
vec2 rayMarch(vec3 ro, vec3 rd){
    float t = 0.0; //total distance travelled
	float id;
	
    for(int i = 0; i < MAX_STEPS; i++) {
        vec3 pos = ro + t * rd; // hit point
        vec2 m = map(pos);
		
        if(abs(m.x) < MIN_DIST){
			id = m.y;
            return vec2(t, id);
		}

		if (abs(m.x) > MAX_DIST){
			break;
		}

        t += m.x;
    }

	return vec2(-1.0, -1.0); // hit sky
}

//color function, using phong lighting model and all other effects
vec3 calcColor(vec3 rayOrigin, vec3 rayDirection, vec2 t){
	vec3 col;
	
	vec3 normal = calcNormal(rayOrigin);
	vec3 lightPosition;

	if (scene != 4)
		lightPosition = vec3(0,2,0);
	else
		lightPosition = vec3(0.5, 5.5, -5.0);

	vec3 light = normalize(lightPosition - rayOrigin);
	vec3 H = reflect(-light, normal);
	vec3 V = -rayDirection;

	vec3 specularColor = vec3(0.5);
	float specularPower = 10.0;
	vec3 specular = specularColor * pow(clamp(dot(H, V), 0.0, 1.0), specularPower);
	vec3 material = getMaterial(rayOrigin, t.y);

	float diffuse;
	diffuse = clamp(dot(normal, normalize(lightPosition - rayOrigin)), 0.0, 1.0);
	diffuse *= 5.0 / dot(light - rayOrigin, light - rayOrigin);
	vec3 ambient = material * 0.25;

	float fresnel = 0.25 * pow(1.0 + dot(rayDirection, normal), 3.0);

	float occ;

	if (ambientOcclusion)
		occ = getOcclusion(rayOrigin, normal);
	else
		occ = 1;

	vec3 back = vec3(1) * 0.05 * clamp(dot(normal, -light), 0.0, 1.0);

	float d = softshadow(rayOrigin + normal * 0.025, light, 0.01, MAX_STEPS, 14);
		
	float diff = max(dot(calcNormal(rayOrigin), light), 0.);

	col = material * ((back + ambient + fresnel) * occ + (vec3(pow(diffuse, 0.4545)) + (specular * occ)) * d);

	if (fogEnabled){
		float fog = smoothstep(4.0, 50.0, t.x) * fogVisibility;
		col = mix(col, background, fog);
	}

	return col;
}

vec3 render(vec2 uv)
{
	vec3 rayOrigin;

	if (scene == 1 || scene == 3){
		background = vec3(0.5,0.7,0.9);
	}
	else
		background = vec3(1.0,1.0,1.0);

	if (scene != 4)
		rayOrigin = cameraPos;
	else
		rayOrigin = vec3(0.0, 2, -9);

	//the direction of our ray, which is computed for every single pixel of the screen (which is also fragCoord.xy/the fragment COORDINATES (one for every pixel))
	vec3 rayDirection;
	
	if (scene != 4)
		rayDirection = getCam(rayOrigin) * normalize(vec3(uv, FOV));
	else{
		vec3 cu = vec3(0, 1, 0);
		vec3 cv = vec3(0.0, 0.0, 1.0);
		vec3 rov = normalize(cv-rayOrigin);
		vec3 u =  normalize(cross(cu, rov));
		vec3 v =  normalize(cross(rov, u));
		rayDirection = normalize(rov + u*uv.x + v*uv.y);
	}

	vec2 t;
	t = rayMarch(rayOrigin, rayDirection);

	rayOrigin += rayDirection * t.x;

	vec3 sceneColor;
	
	if (t.y != -1.0){
		if (useLighting){

			//phong lighting (with gamma correction)
			sceneColor = calcColor(rayOrigin, rayDirection, t);

			if (reflections){
				rayDirection = reflect(rayDirection, calcNormal(rayOrigin));

				t = rayMarch(rayOrigin + rayDirection * MIN_DIST, rayDirection);
				
				rayOrigin += rayDirection * t.x;

				if (rayOrigin.y > -0.99 && t.y == -1.0)
					sceneColor += background * reflectionVisibility;
				else{
					sceneColor += calcColor(rayOrigin, rayDirection, t) * reflectionVisibility;
				}
					
			}
		}
		else{
			sceneColor = getMaterial(rayOrigin, t.y);

			if (fogEnabled){
				float fog = smoothstep(4.0, 50.0, t.x);
				sceneColor = mix(sceneColor, background, fog);
			}
		}
	}
	else{
		sceneColor += background;
	}

	return sceneColor;
}

void main() 
{
	vec3 col;

	//optional anti aliasing, using 4 samples per pixel
	if (antiAliasing){
		col += render(getUV(vec2(0.125, -0.375)));
		col += render(getUV(vec2(-0.125, 0.375)));
		col += render(getUV(vec2(-0.375, 0.125)));
		col += render(getUV(vec2(0.375, -0.125)));
		col /= 4.0;
	}
	else{
		vec2 uv = (gl_FragCoord.xy - 0.5 * resolution.xy) / resolution.y;
		col = render(uv);
	}

	FragColor = vec4(col, 1);
}