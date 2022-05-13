#include "texture-utils.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <glm/glm.hpp>

#include <iostream>

our::Texture2D* our::texture_utils::empty(GLenum format, glm::ivec2 size){
    our::Texture2D* texture = new our::Texture2D();
    //TODO: (Req 10) Finish this function to create an empty texture with the given size and format
    texture->bind();

    // target : specifies the target texture which is here GL_TEXTURE_2D
    // mip level  : specifies the level-of-detail number. Level 0 is the base  level.
    // internal format :Specifies the number of color components in the texture (ex:RGB ,RGBA)
    // width : Specifies the width of the texture
    // height : Specifies the height of the texture
    if(format==GL_RGBA8){
    int levels = (int)glm::floor(glm::log2((float)glm::max(size.x, size.y))) + 1;    
    glTexStorage2D(GL_TEXTURE_2D, levels, format, size.x, size.y);
    }else{
    glTexStorage2D(GL_TEXTURE_2D, 1, format, size.x, size.y);    
    }
    
    return texture;
}

our::Texture2D* our::texture_utils::loadImage(const std::string& filename, bool generate_mipmap) {
    glm::ivec2 size;
    int channels;
    //Since OpenGL puts the texture origin at the bottom left while images typically has the origin at the top left,
    //We need to till stb to flip images vertically after loading them
    stbi_set_flip_vertically_on_load(true);
    //Load image data and retrieve width, height and number of channels in the image
    //The last argument is the number of channels we want and it can have the following values:
    //- 0: Keep number of channels the same as in the image file
    //- 1: Grayscale only
    //- 2: Grayscale and Alpha
    //- 3: RGB
    //- 4: RGB and Alpha (RGBA)
    //Note: channels (the 4th argument) always returns the original number of channels in the file
    unsigned char* pixels = stbi_load(filename.c_str(), &size.x, &size.y, &channels, 4);
    if(pixels == nullptr){
        std::cerr << "Failed to load image: " << filename << std::endl;
        return nullptr;
    }
    // Create a texture
    our::Texture2D* texture = new our::Texture2D();
    //Bind the texture such that we upload the image data to its storage
    //TODO: (Req 4) Finish this function to fill the texture with the data found in "pixels" and generate the mipmaps if requested

    ///////////////////////////////////////////////////////////////

    // glActiveTexture(GL_TEXTURE0);  works without it .. it seems that in testure-test-state.hpp , it was called there but so does the binding so why binding is a must?
    texture->bind();
    // send image data to our texture 
    // target : specifies the target texture which is here GL_TEXTURE_2D
    // level  : specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap reduction image  
    // internal format :Specifies the number of color components in the texture (ex:RGB ,RGBA)
    // width : Specifies the width of the texture image
    // height : Specifies the height of the texture image
    // border : This value must be 0
    // format : Specifies the format of the pixel data
    // type: Specifies the data type of the pixel data
    // data: Specifies a pointer to the image data in memory. 
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)pixels);
    
    if (generate_mipmap){

        // if needed we will generate mipmap 
        // which has several levels .The default is 0 which is of the same size of the image 
        // say the image is 8*8 so level 0 has 8*8 dimensions
        // by increasing the levels , the pixels got reduced half the dimansions by averaging
        // so level 1 has 4*4 dimensions & level 2 has 2 *2  and lastly level 3 is 1 pixel  
      glGenerateMipmap(GL_TEXTURE_2D);
    }
    //////////////end of my code /////////////////


    stbi_image_free(pixels); //Free image data after uploading to GPU
    return texture;
}