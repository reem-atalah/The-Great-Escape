#version 330 core

in Varyings {
    vec4 color;
    vec2 tex_coord;
} fs_in;

out vec4 frag_color;

uniform vec4 tint;
uniform sampler2D tex;

void main(){
    //TODO: (Req 6) Modify the following line to compute the fragment color
    // by multiplying the tint with the vertex color and with the texture color 
    
    // Sample the texture color at the corresponding texture coordinate
    vec4 texture_color = texture(tex, fs_in.tex_coord);
    // Muliply the tint, texture_color and the interpolated color from the vshader.
    // And output the final color to the screen buffer.
    frag_color = tint * texture_color * fs_in.color;
}