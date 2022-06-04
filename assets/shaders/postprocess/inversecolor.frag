#version 330

// The texture holding the scene pixels
uniform sampler2D tex;

// Read "assets/shaders/fullscreen.vert" to know what "tex_coord" holds;
in vec2 tex_coord;
out vec4 frag_color;

void main(){
    frag_color = texture(tex, tex_coord);
    frag_color.rgb = vec3(1,1,1)- frag_color.rgb;
}