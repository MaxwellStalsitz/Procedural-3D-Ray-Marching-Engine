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

vec2 mergeResults(vec2 res1, vec2 res2) 
{
    return (res1.x < res2.x) ? res1 : res2;
}

vec2 differenceInResults(vec2 res1, vec2 res2)
{
	return (res1.x > -res2.x) ? res1 : vec2(-res2.x, res2.y);
}

float sdBox( vec3 p, vec3 b )
{
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float sdSphere(vec3 p, float s)
{
  return length(p)-s;
}

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

vec2 demoScene(vec3 rayPosition)
{
	float distanceToSDF;
	//(the distance to a sphere is found by finding the distance from a point to the center of a sphere, then subtracting the radius)

	float ID = 1.0;
	vec3 position = vec3(-1,0,-5);
	float sphereRadius = 1;
	distanceToSDF = sdSphere(rayPosition-position, sphereRadius);
	vec2 sphere1 = vec2(distanceToSDF, ID);

	ID = 3.0;
	sphereRadius = 1;
	position = vec3(2, 0, -3);
	distanceToSDF = sdSphere(rayPosition-position, sphereRadius);
	vec2 sphere2 = vec2(distanceToSDF, ID);

	ID = 1.0;
	sphereRadius = 1;
	position = vec3(-2.5, -0.5, -2.5);
	distanceToSDF = sdBox(rayPosition-position, vec3(0.5,0.5,0.5));
	vec2 box = vec2(distanceToSDF, ID);

	ID = 2.0;
	float planeHeight = 1.0;
	float planeDistance = rayPosition.y + planeHeight;
	distanceToSDF = planeDistance;
	vec2 plane = vec2(distanceToSDF, ID);

	vec2 result = mergeResults(plane, sphere1);
	result = mergeResults(result, sphere2);
	//result = mergeResults(result, box);
	return result;
}

vec2 mandelbulb(vec3 pos) 
{
	vec3 z = pos;
	float dr = 1.0;
	float r = 0.0;
	float theta, phi;
	
	for (int i = 0; i < iterations; i++) {
		r = length(z);
		if (r > 2.0) break;
		
		if (animate){
			theta = acos(z.z/r)+ (time * timeMultiplier);
			phi = atan(z.y,z.x) + (time * timeMultiplier);
		}
		else{
			theta = acos(z.z/r);
			phi = atan(z.y,z.x);
		}
		
		dr =  pow(r, power-1.0) * power*dr + 1.0;
		
		float zr = pow(r, power);
		theta *= power;
		phi *= power;

		z = zr*vec3(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta));
		z+=pos;
	}
	return vec2((0.5*log(r)*r/dr), 1.0);
}

vec2 map(vec3 rayPosition)
{
	vec2 result;

	if (scene == 1)
		result = demoScene(rayPosition);
	else if (scene == 2)
		result = mandelbulb(rayPosition);
	else if (scene == 3)
		result = customScene(rayPosition);
	
	return result;
}

vec3 getMaterial(vec3 p, float id) 
{
    vec3 m;

	if (id == 0.0)
		//custom material
		id = 0.0;
	else if (id == 1.0)
		m = vec3(1.0, 1.0, 1.0);
	else if (id == 2.0)
		m = vec3(0.2 + 0.4 * mod(floor(p.x) + floor(p.z), 2.0));
	else if (id == 3.0)
		m = vec3(1.0, 0.0, 0.0);
	
    return m;
}

float getOcclusion(vec3 pos, vec3 normal)
{
	float occ = 0.0;
	float weight = 1.0;
	for (int i = 0; i < samples; i++){
		float len = 0.01 + 0.02 * float(i*i);
		float dist = map(pos+normal*len).x;
		occ += (len-dist)*weight;
		weight *= 0.85;
	}
	return 1.0 - clamp(0.6 * occ, 0.0, 1.0);
}

mat3 getCam(vec3 rayOrigin)
{
	vec3 camR = normalize(cross(vec3(0,1,0),cameraFront));
	vec3 camU = cross(cameraFront,camR);
	return mat3(-camR, camU, cameraFront);
}

vec3 calcNormal(vec3 position)
{
	vec2 e = vec2(1.0, -1.0) * 0.0005;
	return normalize(
		e.xyy * map(position + e.xyy).x +
		e.yyx * map(position + e.yyx).x +
		e.yxy * map(position + e.yxy).x +
		e.xxx * map(position + e.xxx).x);
}

float softshadow(vec3 ro, vec3 rd, float mint, float maxt, float k )
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

vec3 fogColor = vec3(0.3, 0.36, 0.6);

vec2 getUV(vec2 offset){
	return ((gl_FragCoord.xy + offset) - 0.5 * resolution.xy) / resolution.y;
}

vec2 traceRef(vec3 ro, vec3 rd){
    
    vec2 t = vec2(0), d;
    
    for (int i = 0; i < MAX_STEPS; i++){

        d = map(ro + rd* t.x);
        
        if(abs(d.x)<0.002 || t.x>MAX_DIST) break;
        
        t.x += d.x;
		t.y = d.y;
    }
    
    return t;
}

vec3 calcColor(vec3 rayOrigin, vec3 rayDirection, float t, vec3 ro){
	vec3 normal = calcNormal(rayOrigin);
		vec3 lightPosition = vec3(0,2,0);
		vec3 light = normalize(lightPosition - rayOrigin);
		vec3 H = reflect(-light, normal);
		vec3 V = -rayDirection;

		vec3 specularColor = vec3(0.5);
		float specularPower = 32.0;
		vec3 specular = specularColor * pow(clamp(dot(H, V), 0.0, 1.0), specularPower);
		vec3 material = getMaterial(ro, t);

		float diffuse;
		diffuse = clamp(dot(normal, normalize(lightPosition - rayOrigin)), 0.0, 1.0);
		diffuse *= 5.0 / dot(light - rayOrigin, light - rayOrigin);
		vec3 ambient = material * 0.05;

		float fresnel = 0.25 * pow(1.0 + dot(rayDirection, normal), 3.0);

		float occ;

		if (ambientOcclusion)
			occ = getOcclusion(rayOrigin, normal);
		else
			occ = 1;

		vec3 back = vec3(1) * 0.05 * clamp(dot(normal, -light), 0.0, 1.0);

		float d = softshadow(rayOrigin + normal * 0.025, light, 0.01, MAX_STEPS, 14);

		vec3 col = material * ((back + ambient + fresnel) * occ + (vec3(pow(diffuse, 0.4545)) + (specular * occ)) * d);
		
		return col;
}

vec3 render(vec2 uv)
{
	vec3 rayOrigin = cameraPos;
	vec3 background = vec3(0.39,0.58,0.93);
	
	//the direction of our ray, which is computer for every single pixel of the screen (which is also fragCoord.xy/the fragment COORDINATES (one for every pixel))
	vec3 rayDirection = getCam(rayOrigin) * normalize(vec3(uv, FOV));

	vec2 h,t;
	for(int i = 0; i < MAX_STEPS; i++){
		h = map(rayOrigin + t.x * rayDirection);
		t.x += h.x;
		t.y = h.y;
		if(abs(h.x) < MIN_DIST || abs(h.x) > MAX_DIST){
			break;
		}
	}

	rayOrigin += rayDirection * t.x;

	vec3 ro = rayOrigin;

	vec3 sceneColor;
	
	if (h.x < MIN_DIST){
		if (useLighting){

			//phong lighting (with gamma correction)
			sceneColor = calcColor(rayOrigin, rayDirection, t.y, ro);

			if (reflections){
				rayDirection = reflect(rayDirection, calcNormal(rayOrigin));

				t = traceRef(rayOrigin + calcNormal(rayOrigin)*0.003, rayDirection);

				rayOrigin += rayDirection * t.x;

				sceneColor += calcColor(rayOrigin, rayDirection, t.y, ro)*0.5;

			}
		}
		else{
			sceneColor = getMaterial(rayOrigin, t.y);
		}
	}
	else{
		sceneColor = background;
	}
	

	return sceneColor;
}

void main() 
{
	vec3 col;

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