#version 330

// The texture holding the scene pixels
uniform sampler2D tex;

// Read "assets/shaders/fullscreen.vert" to know what "tex_coord" holds;
in vec2 tex_coord;

out vec4 frag_color;

// Vignette is a postprocessing effect that darkens the corners of the screen
// to grab the attention of the viewer towards the center of the screen

void main(){
    //TODO: Modify this shader to apply vignette
    // To apply vignette, divide the scene color
    // by 1 + the squared length of the 2D pixel location the NDC space
    // Hint: remember that the NDC space ranges from -1 to 1
    // while the texture coordinate space ranges from 0 to 1
    // We have the pixel's texture coordinate, how can we compute its location in the NDC space?
    //frag_color = vec4(0.0, 0.0, 0.0, 1.0);

    //tex: Texture Sampler has the color
    // tex_coord: Texture Coordinate
    frag_color=texture(tex, tex_coord);
    vec2 NDC_space= (tex_coord*2)-1; // from (ranges from 0 to 1) to (ranges from -1 to 1);
    float d=  NDC_space.x *  NDC_space.x +  NDC_space.y *  NDC_space.y;  //squared length of the pixel location (x*x+y*y)
    frag_color.rgb=vec3(frag_color.rgb)/(1+d);
}