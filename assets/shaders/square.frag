#version 330 core

out vec4 frag_color;

uniform float center;
uniform vec2 side_length;
//TODO: Define uniforms for the center and the side-length

uniform vec4 inside_color; //= vec4(1.0, 0.0, 0.0, 1.0);
uniform vec4 outside_color; //= vec4(0.0, 0.0, 0.0, 1.0);

void main(){
    vec4 cent= vec4(center,0,0);
    //frag_color = inside_color;
    //TODO: Write code that will draw the square
    if(gl_FragCoord.x <= (cent.x+side_length/2) &&  gl_FragCoord.x >= (cent.x-side_length/2) && gl_FragCoord.y <= (cent.y+side_length/2) && gl_FragCoord.y >= (cent.y-side_length/2) ){
    //if(gl_FragCoord.xy- center < vec2(side_length,side_length) ){
        frag_color = inside_color;
    } else {
        frag_color = outside_color;
    }
}