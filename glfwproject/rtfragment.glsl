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

uniform bool useLighting;
uniform bool reflections;

uniform int scene;

float MAX_DIST = 500;
vec3 background = vec3(0.5,0.7,0.9);

const int numberOfSpheres = 3;
vec4 sphere[numberOfSpheres];

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
    float k = 50;

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
        res = min( res, 8.0*sphSoftShadow(ro,rd,sphere[i]) );

    return res;					  
}

float occlusion( in vec3 pos, in vec3 nor )
{
	float res = 1.0;
	for( int i=0; i<numberOfSpheres; i++ )
	    res *= 1.0 - sphOcclusion( pos, nor, sphere[i] ); 

    return res;					  
}

vec3 lig = vec3( 0.0, 2.0, 0.0);

vec3 shade( in vec3 rd, in vec3 pos, in vec3 nor, in float id, in vec3 uvw, vec2 uv, vec3 mate)
{
    vec3 col;
	
	vec3 normal = nor;

	vec3 lp;
	lp = lig;

	vec3 light = normalize(lp - pos);
	vec3 H = reflect(-light, normal);
	vec3 V = -rd;

	vec3 specularColor = vec3(0.5);
	float specularPower = 10.0;
	vec3 specular = specularColor * pow(clamp(dot(H, V), 0.0, 1.0), specularPower);
	vec3 material = mate;

	float diffuse;
	diffuse = clamp(dot(normal, normalize(lp - pos)), 0.0, 1.0);
	diffuse *= 5.0 / dot(light - pos, light - pos);
	vec3 ambient = material * 0.25;

	float fresnel = 0.25 * pow(1.0 + dot(rd, normal), 3.0);

	float occ;

    float dif = clamp( dot(nor,lig), 0.0, 1.0 );
    float sha = 1.0;
        if( dif>0.001 ) sha = shadow( pos, lig );

	if (ambientOcclusion){
            occ = occlusion( pos, nor );
            occ = occ*0.5 + 0.5*occ*occ;
    }
    else
        occ = 1;

	vec3 back = vec3(1) * 0.05 * clamp(dot(normal, -light), 0.0, 1.0);

	float diff = max(dot(normal, light), 0.);

	col = material * ((back + ambient + fresnel) * occ + (vec3(pow(diffuse, 0.4545)) + (specular * occ)) * sha);

	return col;	
}  

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 trace( in vec3 ro, in vec3 rd, vec3 col, in float tmin, vec2 uv)
{
	float t = tmin;
	float id  = -1.0;
    vec4  obj = vec4(0.0);

    int refl = 1;
    if (reflections)
        refl = 2;

    for (int i = 0; i < refl; i++){
        vec3 passCol;

        t = tmin;
        id  = -1.0;
        obj = vec4(0.0);

	    for( int i=0; i<numberOfSpheres; i++ )
	    {
		    vec4 sph = sphere[i];
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

            passCol = shade( rd, pos, nor, id, pos-obj.xyz, uv, vec3(0.61, 0.176, 0.176)); 
        }
        else{
            //distance to plane
            float t = (-1.0-ro.y)/rd.y; //-1.0 = height
            if( t > -1.0)
            {
                tmin = t;

                float distToPoint = distance(pos, vec3(ro+t*rd));

                pos = ro + t*rd;
                nor = vec3(0.0,1.0,0.0); // plane
                vec3 tile = mix(vec3(0.0 + 1.0 * mod(floor(pos.x) + floor(pos.z), 2.0)), vec3(0.773, 0.725, 0.627), 0.5); //from inigo quilez (with modified colors)

                if (i == 0){
                    rd = reflect (rd, nor);
                    ro = pos;

                    t = tmin;
                    id  = -1.0;
                    obj = vec4(0.0);

                    for( int i=0; i<numberOfSpheres; i++ )
	                {
		                vec4 sph = sphere[i];
	                    float h = sphIntersect( ro, rd, sph); 
		                if( h>0.0 && h<t )
		                {
			                t = h;
                            obj = sph;
			                id = float(i);	
                        }
	                }

                    if (id == -1.0){
				        return shade( rd, pos, nor, id, pos, uv, tile);
                    }
                }

                passCol = shade( rd, pos, nor, id, pos, uv, tile);
            }
            else{
                if (i == 0)
                    return background;
					
                passCol = background;
			}
        }

        if (i == 0)
            col = passCol;
        else
		    col += mix(col, passCol, 0.5);
        
        
        rd = reflect (rd, nor);
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
	    sphere[i] = vec4(vec3(x*2,0.0,z*2), ra);
    }
}

void defaultScene(){
    float ra = pow(2/float(numberOfSpheres-1),4.0);
    ra = 0.2 + 0.8*ra;
    ra *= 1.0; //scale

	vec3 position = vec3(-.5,0.0,-6);
	sphere[0] = vec4(position, ra);
    position = vec3(-3.0, 0.0, -3.0);
    sphere[1] = vec4(position, ra);
    position = vec3(2, 0, -3);
    sphere[2] = vec4(position, ra);
}

vec3 render(vec2 uv)
{
	vec2 q = gl_FragCoord.xy / resolution.xy;

	float time = time*0.5;

    defaultScene(); // setting up scene entities

    vec3 ro, rd;
    ro = cameraPos; 
    rd = getCam(ro) * normalize(vec3(uv, 1.0));

	vec3 col = vec3(0);

    float tmin = 1e20;

    col = trace( ro, rd, col, tmin, uv);

    return col;
}

vec2 getUV(vec2 offset){
	return ((gl_FragCoord.xy + offset) - 0.5 * resolution.xy) / resolution.y;
}

void main(){
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

