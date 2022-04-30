#pragma once

#include <glad/gl.h>

namespace our {

    // This class defined an OpenGL texture which will be used as a GL_TEXTURE_2D
    class Texture2D {
        // The OpenGL object name of this texture 
        GLuint name = 0;
    public:
        // This constructor creates an OpenGL texture and saves its object name in the member variable "name" 
        Texture2D() {
            //TODO: (Req 4) Complete this function

            // 1 is the number of texture name to be generated
            // the second parameter specifies an array in which the generated texture names are stored.
            glGenTextures(1, &name);            
        };

        // This deconstructor deletes the underlying OpenGL texture
        ~Texture2D() { 
            //TODO: (Req 4) Complete this function

            // the first parameter Specifies the number of textures to be deleted.
            // the second parameter Specifies an array of textures to be deleted.
            glDeleteTextures(1,&name);
        }

        // Get the internal OpenGL name of the texture which is useful for use with framebuffers
        GLuint getOpenGLName() {
            return name;
        }

        // This method binds this texture to GL_TEXTURE_2D
        void bind() const {
            //TODO: (Req 4) Complete this function

            // lets you create or use a named texture.
            // with the target set to  GL_TEXTURE_2D (or any other type) and texture set to the name of the new texture
            // it binds the texture name to the target.
            // When a texture is bound to a target, the previous binding for that target is automatically broken.
            glBindTexture(GL_TEXTURE_2D, name);
        }

        // This static method ensures that no texture is bound to GL_TEXTURE_2D
        static void unbind(){
            //TODO: (Req 4) Complete this function

            //"unbind" makes sure that no texture object is bound to GL_TEXTURE_2D. 
            // In other words, it binds 0 (which means no object in opengl terms) to GL_TEXTURE_2D.
            glBindTexture(GL_TEXTURE_2D, 0);

        }

        Texture2D(const Texture2D&) = delete;
        Texture2D& operator=(const Texture2D&) = delete;
    };
    
}