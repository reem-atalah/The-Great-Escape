#version 330

// The texture holding the scene pixels
uniform sampler2D tex;

// Read "assets/shaders/fullscreen.vert" to know what "tex_coord" holds;
in vec2 tex_coord;
out vec4 frag_color;

void main(){
    frag_color = texture(tex, tex_coord);
    if(frag_color.r<.3 && frag_color.g<.3 && frag_color.b<.3)
        frag_color.rgb = frag_color.rgb/1.5;
    else
        frag_color.rgb = frag_color.rgb*1.5;    
}