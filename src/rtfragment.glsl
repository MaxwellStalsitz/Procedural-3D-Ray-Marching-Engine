#version 430 core
out vec4 FragColor;

uniform vec2 resolution;
uniform vec2 mouse;
uniform bool antiAliasing;
uniform bool ambientOcclusion;
uniform vec3 cameraPos;
uniform float time;

uniform vec3 direction;
uniform vec3 cameraFront;

uniform bool fogEnabled;
uniform float fogVisibility;
uniform float falloff;

uniform bool useLighting;
uniform vec3 lightPosition;

uniform bool reflections;
uniform float visibility;
uniform int reflectionCount;

uniform int scene;

float MAX_DIST = 500;
vec3 background = vec3(0.5,0.7,0.9);

#define RED 1;
#define GREEN 2;
#define BLUE 3;

const int numberOfSpheres = 3;

struct Sphere{
    vec4 sphere;
    int materialId;
};

Sphere spheres[numberOfSpheres];

vec3 sphNormal( in vec3 pos, in vec4 sph )
{
    return normalize(pos-sph.xyz);
}

float sphIntersect( in vec3 ro, in vec3 rd, in vec4 sph)
{
	vec3 oc = ro - sph.xyz;
	float b = dot( oc, rd );
	float c = dot( oc, oc ) - sph.w*sph.w;
	float h = b*b - c;
	if( h<0.0 ) return -1.0;
	return -b - sqrt( h );
}

// https://iquilezles.org/articles/sphereshadow
float sphShadow( in vec3 ro, in vec3 rd, in vec4 sph )
{
    float k = 14;

    vec3 oc = ro - sph.xyz;
	float b = dot( oc, rd );
	float c = dot( oc, oc ) - sph.w*sph.w;
	float h = b*b - c;
	
	float d = -sph.w + sqrt( max(0.0,sph.w*sph.w-h));
	float t = -b     - sqrt( max(0.0,h) );
	return (t<0.0) ? 1.0 : smoothstep( 0.0, 1.0, k*d/t );
}
            
vec2 sphDistances( in vec3 ro, in vec3 rd, in vec4 sph )
{
	vec3 oc = ro - sph.xyz;
    float b = dot( oc, rd );
    float c = dot( oc, oc ) - sph.w*sph.w;
    float h = b*b - c;
    float d = sqrt( max(0.0,sph.w*sph.w-h)) - sph.w;
    return vec2( d, -b-sqrt(max(h,0.0)) );
}

float sphSoftShadow( in vec3 ro, in vec3 rd, in vec4 sph )
{
    float s = 1.0;
    vec2 r = sphDistances( ro, rd, sph );
    if( r.y>0.0 )
        s = max(r.x,0.0)/r.y;
    return s;
}

float sphOcclusion( in vec3 pos, in vec3 nor, in vec4 sph )
{
    vec3  r = sph.xyz - pos;
    float l = length(r);
    float d = dot(nor,r);
    float res = d;

    if( d<sph.w ) res = pow(clamp((d+sph.w)/(2.0*sph.w),0.0,1.0),1.5)*sph.w;
    
    return clamp( res*(sph.w*sph.w)/(l*l*l), 0.0, 1.0 );
}

float shadow( in vec3 ro, in vec3 rd )
{
	float res = 1.0;
	for( int i=0; i<numberOfSpheres; i++ )
        res = min( res, 8.0*sphSoftShadow(ro,rd,spheres[i].sphere) );

    return res;					  
}

float occlusion( in vec3 pos, in vec3 nor )
{
	float res = 1.0;
	for( int i=0; i<numberOfSpheres; i++ )
	    res *= 1.0 - sphOcclusion( pos, nor, spheres[i].sphere );

    return res;					  
}

vec3 getMaterial(int materialId, vec3 pos){
    vec3 col;

    if (materialId == 0){
        col = mix(vec3(0.0 + 1.0 * mod(floor(pos.x) + floor(pos.z), 2.0)), vec3(0.773, 0.725, 0.627), 0.5); //from inigo quilez (with modified colors)
    }
    else if (materialId == 1){ // red
        col = vec3(0.61, 0.176, 0.176);
    }
    else if (materialId == 2){ // green
        col = vec3(0.49, 0.74, 0.4);
    }
    else if (materialId == 3){ // blue
        col = vec3(0.247, 0.427, 0.819);
    }

    return col;
}

vec3 shade( in vec3 rd, in vec3 pos, in vec3 nor, in float id, in vec3 uvw, vec2 uv, int materialId, float tmin)
{
    vec3 col;
	
    if (useLighting){
	    vec3 normal = nor;

	    vec3 light = normalize(lightPosition - pos);
	    vec3 H = reflect(-light, normal);
	    vec3 V = -rd;

	    vec3 specularColor = vec3(0.5);
	    float specularPower = 10.0;
	    vec3 specular = specularColor * pow(clamp(dot(H, V), 0.0, 1.0), specularPower);
	    vec3 material = getMaterial(materialId, pos);

	    float diffuse;
	    diffuse = clamp(dot(normal, normalize(lightPosition - pos)), 0.0, 1.0);
	    diffuse *= 5.0 / dot(light - pos, light - pos);
	    vec3 ambient = material * 0.25;

	    float fresnel = 0.25 * pow(1.0 + dot(rd, normal), 3.0);

	    float occ;

        float dif = clamp( dot(nor,lightPosition), 0.0, 1.0 );
        float sha = 1.0;
        if( dif>0.001 ) sha = shadow( pos, light );

	    if (ambientOcclusion){
                occ = occlusion( pos, nor );
                occ = occ*0.5 + 0.5*occ*occ;
        }
        else
            occ = 1;

	    vec3 back = vec3(1) * 0.05 * clamp(dot(normal, -light), 0.0, 1.0);

	    col = material * ((back + ambient + fresnel) * occ + (vec3(pow(diffuse, 0.4545) * sha) + (specular * occ)));
    }
    else
        col = getMaterial(materialId, pos);

    if (fogEnabled && id == -1.0){
        float fog = smoothstep(4.0, falloff, tmin) * fogVisibility;
		col = mix(col, background, fog);
    }

	return col;	
}  

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 trace( in vec3 ro, in vec3 rd,  in float tmin, vec2 uv) // here
{
    vec3 col = vec3(0);

	float t = tmin;
	float id  = -1.0;
    vec4 obj = vec4(0.0);

    int refl = reflectionCount;

    if (!reflections) refl = 1;

    for (int i = 0; i < refl; i++){
        vec3 passCol;

        t = tmin;
        id  = -1.0;
        obj = vec4(0.0);

	    for( int i=0; i<numberOfSpheres; i++ )
	    {
		    vec4 sph = spheres[i].sphere;
	        float h = sphIntersect( ro, rd, sph); 
		    if( h>0.0 && h<t ) //t = min distance
		    {
			    t = h;
                obj = sph;
			    id = float(i);	
            }
	    }

        vec3 nor, pos;

        if( id != -1.0)
        {
		    pos = ro + t*rd;
		    nor = sphNormal( pos, obj );

            passCol = shade( rd, pos, nor, id, pos-obj.xyz, uv, spheres[int(id)].materialId, t); 
        }
        else{

            float t = (-1.0-ro.y)/rd.y; //-1.0 = height
            if( t > -1.0){
                tmin = t;

                float distToPoint = distance(pos, vec3(ro+t*rd));

                pos = ro + t*rd;
                nor = vec3(0.0,1.0,0.0);

                passCol = shade( rd, pos, nor, id, pos*0.5, uv, 0, t);

            }
            else{
                if (i == 0) return background; else passCol = background;
			}
        }

        if (i == 0)
            col = passCol;
        else
		    col = mix(passCol, col, visibility);

        rd = normalize(reflect (rd, nor));
        ro = pos;
    }

    return col;
}

mat3 getCam(vec3 rayOrigin)
{
	vec3 camR = normalize(cross(vec3(0,1,0),cameraFront));
	vec3 camU = cross(cameraFront,camR);
	return mat3(-camR, camU, cameraFront);
}

void manyEntityScene(){

    for (int i = 0; i < numberOfSpheres; i++){
        float ra = pow(2/float(numberOfSpheres-1),4.0);
        ra = 0.2 + 0.8*ra;
        ra *= 1.0; // scale

        float x = float(i % 10);
		float z = float(i / 10);
	    spheres[i].sphere = vec4(vec3(x*2,0.0,z*2), ra);
    }
}

void defaultScene()
{
    float ra = pow(2/float(numberOfSpheres-1),4.0);
    ra = 0.2 + 0.8*ra;
    ra *= 1.0; //scale

	vec3 position = vec3(-.5,0.0,-6);
	spheres[0].sphere = vec4(position, ra);
    spheres[0].materialId = GREEN;

    position = vec3(-3.0, 0.0, -3.0);
    spheres[1].sphere = vec4(position, ra);
    spheres[1].materialId = RED;


    position = vec3(2, 0, -3);
    spheres[2].sphere = vec4(position, ra);
    spheres[2].materialId = BLUE;
}

vec3 render(vec2 uv)
{
	vec2 q = gl_FragCoord.xy / resolution.xy;

	float time = time*0.5;

    defaultScene(); // setting up scene entities

    vec3 ro, rd;
    ro = cameraPos; 
    rd = getCam(ro) * normalize(vec3(uv, 1.0));

    float tmin = 1e20;

    vec3 col = trace( ro, rd, tmin, uv);

    return col;
}

vec2 getUV(vec2 offset)
{
	return ((gl_FragCoord.xy + offset) - 0.5 * resolution.xy) / resolution.y;
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

     FragColor = vec4( col, 1.0 );
}

