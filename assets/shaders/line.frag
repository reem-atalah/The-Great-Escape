#version 330 core

out vec4 frag_color;

//TODO: Define uniforms for the slope and the intercept

uniform vec4 inside_color ;
uniform vec4 outside_color ;

uniform float slope;
uniform float intercept;

void main(){
    //TODO: Write code that will draw the square
    if(gl_FragCoord.y <= (gl_FragCoord.x*slope)+intercept){
        frag_color = inside_color;
    } else {
        frag_color = outside_color;
    }
}