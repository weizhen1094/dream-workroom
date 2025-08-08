#version 410 core

layout(location=0) out vec4 FragColor;

struct Material // use std140
{
    vec3 kAmbient;
    float shininess;        
    vec3 kDiffuse;
    float padding;        
    vec3 kSpecular;
    float padding1;  
};

struct Light
{
	vec3 position;
	int type; // point = 0, directional = 1, spot = 2
	vec3 color;
	float intensity;
	vec3 direction; ///< if dir
	float range; ///< if point/spot
	float angle; ///< if spot
};

in vec3 pos;
in vec3 nml;

uniform Material material;
uniform mat4 viewMtx;       
uniform int numLights;
uniform Light light;


vec3 BlinnPhong(vec3 position, vec3 normal, Light light, Material material, mat4 view)
{
    vec3 lightPosView = vec3(view * vec4(light.position, 1.0f));
    if (position == lightPosView) { return vec3(0.0f); }

    vec3 La = light.color * light.intensity;
    vec3 ambient = La * material.kAmbient;

    float attenuation = 1.0f;
    vec3 L; // light dir
    if(light.type == 1)  // directional
    { L = normalize(-light.direction); }
	else if(light.type == 2) // spot
	{
        L = normalize(lightPosView - position);
        // todo..
	}
	else  // point
    { 
        L = normalize(lightPosView - position); 
        float sqredDist = L.x*L.x + L.y*L.y + L.z*L.z;
        attenuation = 1 / max(1, 1.0f * sqredDist);
    }

    vec3 N = normalize(normal);
    float lambert = max(dot(N, L), 0.0f);
    if(lambert <= 0.0f) { return ambient; }
    vec3 Ld = light.color * light.intensity;
    vec3 diffuse = Ld  * material.kDiffuse * lambert * attenuation;
    
    vec3 V = normalize(-position);
    vec3 H = normalize(L + V);
    vec3 Ls = light.color * light.intensity;
    vec3 specular = Ls * material.kSpecular * pow(max(dot(N, H), 0.0f), material.shininess) * attenuation;
    
    return ambient + diffuse + specular;
}

void main() 
{
    FragColor = vec4(BlinnPhong(pos, normalize(nml), light, material, viewMtx), 1.0f);
}