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

float MAX_DIST = 500;

vec3 background = vec3(0.5,0.7,0.9);

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

#define NUMSPHEREES 3

vec4 sphere[NUMSPHEREES];

float shadow( in vec3 ro, in vec3 rd )
{
	float res = 1.0;
	for( int i=0; i<NUMSPHEREES; i++ )
        res = min( res, 8.0*sphSoftShadow(ro,rd,sphere[i]) );

    return res;					  
}

float occlusion( in vec3 pos, in vec3 nor )
{
	float res = 1.0;
	for( int i=0; i<NUMSPHEREES; i++ )
	    res *= 1.0 - sphOcclusion( pos, nor, sphere[i] ); 

    return res;					  
}

vec3 lig = normalize( vec3( -0.8, 0.3, -0.5 ) );

vec3 shade( in vec3 rd, in vec3 pos, in vec3 nor, in float id, in vec3 uvw, in float dis, vec2 uv, vec3 mate)
{
    vec3 finalColor;
    if (useLighting){
        vec3 light = normalize(lig - pos);

        vec3 ref = reflect(rd, nor);
        float fre = 0.25 * pow(1.0 + dot(rd, nor), 3.0);
        float occ;
    
        if (ambientOcclusion){
            occ = occlusion( pos, nor );
            occ = occ*0.5 + 0.5*occ*occ;
        }
        else
            occ = 1;
    
        float dif = clamp( dot(nor,lig), 0.0, 1.0 );

        float diffuse;
	    diffuse = clamp(dot(nor, normalize(lig - pos)), 0.0, 1.0);
	    diffuse *= 5.0 / dot(light - pos, light - pos);

        float sha = 1.0;
        if( dif>0.001 ) sha = shadow( pos, lig );

        vec3 bac = vec3(1) * 0.05 * clamp(dot(nor, -light), 0.0, 1.0);

        vec3 H = reflect(-light, nor);
	    vec3 V = -rd;

        vec3 specularColor = vec3(0.5);
	    float specularPower = 10.0;
        vec3 specular = specularColor * pow(clamp(dot(H, V), 0.0, 1.0), specularPower);
		
        vec3 qe = vec3(0.5,0.5,0.5);

        vec3 ambient = mate * 0.05;

        vec3 col = mate * ((bac + ambient + fre) * occ + (vec3(pow(diffuse, 0.4545)) + (specular * occ)) * sha); //+refval

        float r = clamp(qe.x,0.0,1.0);

        if (fogEnabled){
            float fog = smoothstep(4.0, 50.0, dis) * fogVisibility;
	        col = mix(col, background, fog);
        }


        finalColor = col;
    }
    else
        finalColor = mate;

    return finalColor;
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

	    for( int i=0; i<NUMSPHEREES; i++ )
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

        vec3 nor, pos;

        if( id != -1.0)
        {
		    pos = ro + t*rd;
		    nor = sphNormal( pos, obj );

            passCol = shade( rd, pos, nor, id, pos-obj.xyz, 0.01, uv, vec3(0.5)); 
        }
        else{
            float t = (-1.0-ro.y)/rd.y; //-1.0 = height
            if( t>0.0 )
            {
                tmin = t;
                vec3 pos = ro + t*rd;
                vec3 nor = vec3(0.0,1.0,0.0); // plane
                vec3 tile = mix(vec3(0.0 + 1.0 * mod(floor(pos.x) + floor(pos.z), 2.0)), vec3(0.773, 0.725, 0.627), 0.5); //from inigo quilez

                if (i == 0)
                    return shade( rd, pos, nor, 0.0, pos*0.5, t, uv, tile);
                else
                    passCol = shade( rd, pos, nor, 0.0, pos*0.5, t, uv, tile);
            }
            else{
                if (i == 0)
                    return background;
                else
                    passCol = background;
			}
                
        }

        if (i == 0){
            col = passCol;
            rd = reflect (rd, nor);
            ro = pos;
        }
        else
            col += mix(col, passCol, 0.5);
    }

    return col;
}

mat3 getCam(vec3 rayOrigin)
{
	vec3 camR = normalize(cross(vec3(0,1,0),cameraFront));
	vec3 camU = cross(cameraFront,camR);
	return mat3(-camR, camU, cameraFront);
}

vec3 render(vec2 uv)
{
	vec2 q = gl_FragCoord.xy / resolution.xy;

	float time = time*0.5;


    for (int i = 0; i < NUMSPHEREES; i++){
        float ra = pow(2/float(NUMSPHEREES-1),4.0);
        ra = 0.2 + 0.8*ra;
        ra *= 1.0; // scale

        float x = float(i % 10);
		float z = float(i / 10);
	sphere[i] = vec4(vec3(x*2,0,z*2), ra);
    }

    float le = 2.0;
    float px = 1.0*(2.0/resolution.y)*(1.0/le);

    vec3 ro, rd;

    ro = cameraPos;

    rd = getCam(ro) * normalize(vec3(uv, 1.0));

	vec3 col = vec3(1);

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

