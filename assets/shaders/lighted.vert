#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in vec3 normal;

uniform mat4 M;
uniform mat4 M_IT;
uniform vec3 eye;

out Varyings {
    vec4 color;
    vec2 tex_coord;
    vec3 normal;
    vec3 view;
    vec3 world;
} vs_out;

uniform mat4 transform;

void main(){
    //TODO: (Req 6) Change the next line to apply the transformation matrix
    vec3 world = (M * vec4(position, 1.0)).xyz;
    
    // Transform the position by mulitplying it with the transform matrix
    gl_Position = transform * vec4(position, 1.0);
    // Sending the color and tex_coord to the fshader
    vs_out.normal = normalize((M_IT * vec4(normal, 0.0)).xyz);
    vs_out.view = eye - world;
    vs_out.color = color;
    vs_out.tex_coord = tex_coord;
    vs_out.world = world;
}