#version 330 core

out vec4 frag_color;

//Defining uniforms for the center and the radius
uniform vec2 center;
uniform float radius;
            
uniform vec4 inside_color = vec4(1.0, 0.0, 0.0, 1.0);
uniform vec4 outside_color = vec4(0.0, 0.0, 0.0, 1.0);

void main(){
    // Defining the x and y coordinates of the center point
    float center_x = center[0];
    float center_y = center[1];
    
    // Calculating the euclidian distance between circle center and the fragment pixel 
    float distance = sqrt(pow(gl_FragCoord.x - center_x , 2.0) + pow(gl_FragCoord.y - center_y , 2.0));
     
    if (distance <= radius){
        // If pixel distance from center is smaller than the raduis
        // The pixel is inside the circle
        frag_color = inside_color;
    } else {
        frag_color = outside_color;
    }
}