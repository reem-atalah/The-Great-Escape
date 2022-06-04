#version 330 core

#define MAX_LIGHTS 16
#define DIRECTIONAL 1
#define POINT 2
#define SPOT 3

struct Light {
    int type;
    vec3 position;
    vec3 direction;
    vec3 diffuse;
    vec3 specular;
    vec3 attenuation; // x*d^2 + y*d + z
    vec2 cone_angles; // x: inner_angle, y: outer_angle
};

uniform Light lights[MAX_LIGHTS];
uniform int light_count;

in Varyings {
    vec4 color;
    vec2 tex_coord;
    vec3 normal;
    vec3 view;
    vec3 world;
} fs_in;

out vec4 frag_color;

//uniform vec4 tint;
//uniform sampler2D tex;
struct Sky {
    vec3 top, middle, bottom;
};

uniform Sky sky;

struct Material {
    sampler2D albedo;
    sampler2D specular;
    sampler2D ambient_occlusion;
    sampler2D roughness;
    sampler2D emissive;
};

uniform Material material;



void main(){
    // normalize the view and the normal
    vec3 view = normalize(fs_in.view);
    vec3 normal = normalize(fs_in.normal);

    vec3 material_diffuse = texture(material.albedo, fs_in.tex_coord).rgb;
    vec3 material_specular = texture(material.specular, fs_in.tex_coord).rgb;
    vec3 material_ambient = material_diffuse * texture(material.ambient_occlusion, fs_in.tex_coord).r;

    float material_roughness = texture(material.roughness, fs_in.tex_coord).r;
    float material_shininess = 2.0 / pow(clamp(material_roughness, 0.001, 0.999), 4.0) - 2.0;

    // get the top or buttom light of the sky according to normal direction
    vec3 sky_light = (normal.y > 0) ?
        mix(sky.middle, sky.top, normal.y * normal.y) :
        mix(sky.middle, sky.bottom, normal.y * normal.y);

    vec3 material_emissive = texture(material.emissive, fs_in.tex_coord).rgb;
    // get the initial value for the color
    frag_color = vec4(material_emissive + material_ambient * sky_light , 1.0);

    int clamped_light_count = min(MAX_LIGHTS, light_count);
    // loop over all lights to get the effect for all of them 
    for(int i = 0; i < clamped_light_count; i++){
        Light light = lights[i];

        vec3 direction_to_light = -light.direction;
        if(light.type != DIRECTIONAL){
            // if the light spot or point not use the diraction vector 
            direction_to_light = normalize(light.position - fs_in.world);
        }
        // get diffused light
        vec3 diffuse = light.diffuse * material_diffuse * max(0, dot(normal, direction_to_light));
        // get the reflected light direction 
        vec3 reflected = reflect(-direction_to_light, normal);
        // get specular light
        vec3 specular = light.specular * material_specular * pow(max(0, dot(view, reflected)), material_shininess);

        float attenuation = 1;
        if(light.type != DIRECTIONAL){
            // if light is point or spot the distance is affected the attenuation 
            float d = distance(light.position, fs_in.world);
            attenuation /= dot(light.attenuation, vec3(d*d, d, 1));
            // if light is spot the the object not affected by light if it is outside the outer cone angle
            if(light.type == SPOT){
                float angle = acos(dot(-direction_to_light, light.direction));
                attenuation *= smoothstep(light.cone_angles.y, light.cone_angles.x, angle);
            }
        }

        frag_color.rgb += (diffuse + specular) * attenuation;
    }
}