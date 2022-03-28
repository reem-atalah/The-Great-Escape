#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace our {

    class ShaderProgram {

    private:
        //Shader Program Handle
        GLuint program;

    public:
        void create();
        void destroy();

        ShaderProgram(){ program = 0; }
        ~ShaderProgram(){ destroy(); }

        bool attach(const std::string &filename, GLenum type) const;

        bool link() const;

        void use() { 
            //TODO: call opengl to use the program identified by this->program
            glUseProgram(this->program);
        }

        GLuint getUniformLocation(const std::string &name) {
            //TODO: call opengl to get the uniform location for the uniform defined by name from this->program
            // Change the string to a char ptr
            const char *name_str = name.c_str();
            // Get the location of that name from the uniform memory
            return glGetUniformLocation(this->program,name_str);
        }

        void set(const std::string &uniform, GLfloat value) {
            //TODO: call opengl to set the value to the uniform defined by name
            GLuint Location=getUniformLocation(uniform);
            // Write the value read from the json, into the location of the corresponding uniform
            glUniform1f(Location,value);
        }

        void set(const std::string &uniform, glm::vec2 value) {
            //TODO: call opengl to set the value to the uniform defined by name
            GLuint Location=getUniformLocation(uniform);
            //float fvalue[]={value[0],value[1]};
            glUniform2fv(Location,1,value_ptr(value));
        }

        void set(const std::string &uniform, glm::vec3 value) {
            //TODO: call opengl to set the value to the uniform defined by name
            GLuint Location=getUniformLocation(uniform);
            
            // First parameter: take the location of the uniform
            // Second Parameter: the number of array elements to be passed.
            // Third Parameter: pointer to the value to be put in that location
            glUniform3fv(Location,1,value_ptr(value));
        }

        void set(const std::string &uniform, glm::vec4 value) {
            //TODO: call opengl to set the value to the uniform defined by name
             GLuint Location=getUniformLocation(uniform);
            
            // Write a vec4 value read from the json, into the location of the corresponding uniform
            // e.g. take the value of the inside_color vector from the json file,
            // and put it in the location of the inside_color uniform in the fragment shader.
            glUniform4fv(Location,1,value_ptr(value));
        }


        //TODO: Delete the copy constructor and assignment operator
        ShaderProgram (const ShaderProgram &) = delete;
        ShaderProgram & operator= (const ShaderProgram&) = delete;
        //Question: Why do we do this? Hint: Look at the deconstructor :)
        //To disallow copying objects, so that no 2 objects have the same program id.
        //This is done to prevent the case of having an object of shaderprogram pointing to a deleted program already. 
    };

}

#endif