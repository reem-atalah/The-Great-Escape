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
            const char *name_str = name.c_str();
            return glGetUniformLocation(this->program,name_str);
        }

        void set(const std::string &uniform, GLfloat value) {
            //TODO: call opengl to set the value to the uniform defined by name
            GLuint Location=getUniformLocation(uniform);
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
            //float fvalue[]={value[0],value[1],value[2],value[3]};
            glUniform3fv(Location,1,value_ptr(value));
        }

        void set(const std::string &uniform, glm::vec4 value) {
            //TODO: call opengl to set the value to the uniform defined by name
             GLuint Location=getUniformLocation(uniform);
            //float fvalue[]={value[0],value[1],value[2],value[3]};
            glUniform4fv(Location,1,value_ptr(value));
        }


        //TODO: Delete the copy constructor and assignment operator
        //From internet :)
        ShaderProgram (const ShaderProgram &) = delete;
        ShaderProgram & operator= (const ShaderProgram&) = delete;
        //Question: Why do we do this? Hint: Look at the deconstructor
    };

}

#endif