#pragma once

#include <glad/gl.h>
#include "vertex.hpp"

namespace our {

    #define ATTRIB_LOC_POSITION 0
    #define ATTRIB_LOC_COLOR    1
    #define ATTRIB_LOC_TEXCOORD 2
    #define ATTRIB_LOC_NORMAL   3

    class Mesh {
        // Here, we store the object names of the 3 main components of a mesh:
        // A vertex array object, A vertex buffer and an element buffer
        unsigned int VBO, EBO;
        unsigned int VAO;
        // We need to remember the number of elements that will be draw by glDrawElements 
        GLsizei elementCount;        
    public:
        std::vector<Vertex> mesh_vertices;
        std::vector<GLuint> mesh_elements;

        // The constructor takes two vectors:
        // - vertices which contain the vertex data.
        // - elements which contain the indices of the vertices out of which each rectangle will be constructed.
        // The mesh class does not keep a these data on the RAM. Instead, it should create
        // a vertex buffer to store the vertex data on the VRAM,
        // an element buffer to store the element data on the VRAM,
        // a vertex array object to define how to read the vertex & element buffer during rendering 
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& elements): mesh_vertices(vertices)
        {

            mesh_elements=elements;
            //TODO: (Req 1) Write this function
            // remember to store the number of elements in "elementCount" since you will need it for drawing
            // For the attribute locations, use the constants defined above: ATTRIB_LOC_POSITION, ATTRIB_LOC_COLOR, etc
            glGenVertexArrays(1,&VAO); 
            glBindVertexArray(VAO);

            
            glGenBuffers(1,&VBO);
            glBindBuffer(GL_ARRAY_BUFFER,VBO);
            glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(Vertex),(void*) vertices.data(),GL_STATIC_DRAW); //GL_STATIC_DRAW -> data will not change

            // take the index of the attribArray
            glEnableVertexAttribArray(ATTRIB_LOC_POSITION);
            //GLuint index->index of the attribArray
            //GLint size ->nuber of elements for the positions
            //GLenum type -> type of position element,
            //GLboolean normalized -> flase not apply normalization for values
            //GLsizei stride ->jump how much every time 
            //void *pointer ->offset from begin
            glVertexAttribPointer(ATTRIB_LOC_POSITION,3,GL_FLOAT,false,sizeof(Vertex),(void*) offsetof(Vertex,position));

            
            glEnableVertexAttribArray(ATTRIB_LOC_COLOR);
            //GLboolean normalized -> true, apply normalization for values
            glVertexAttribPointer(ATTRIB_LOC_COLOR,4,GL_UNSIGNED_BYTE,true,sizeof(Vertex),(void*) offsetof(Vertex,color));
            

            
            glEnableVertexAttribArray(ATTRIB_LOC_TEXCOORD);
            glVertexAttribPointer(ATTRIB_LOC_TEXCOORD,2,GL_FLOAT,false,sizeof(Vertex),(void*) offsetof(Vertex,tex_coord));
            

            
            glEnableVertexAttribArray(ATTRIB_LOC_NORMAL);
            glVertexAttribPointer(ATTRIB_LOC_NORMAL,3,GL_FLOAT,false,sizeof(Vertex),(void*) offsetof(Vertex,normal));
           

            elementCount=elements.size();
            glGenBuffers(1,&EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
            //GLenum target -> GL_ELEMENT_ARRAY_BUFFER
            //GLsizeiptr size ->size of elements in number of bytes,
            //const void *data -> pointer to data
            //GLenum usage -> GL_STATIC_DRAW -> data will not change
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,elementCount*sizeof(unsigned int),(void*)elements.data(),GL_STATIC_DRAW); 

            //glBindVertexArray(0);
        }

        // this function should render the mesh
        void draw() 
        {
            //TODO: (Req 1) Write this function
            // Bind Vertix array
            glBindVertexArray(VAO);
            //GLenum mode -> GL_TRIANGLES
            //GLsizei count-> count of elements use to drow
            //GLenum type -> type of elements is unsigned int
            //const void *indices-> offset from begin of elements 
            glDrawElements(GL_TRIANGLES,elementCount,GL_UNSIGNED_INT,(void*)0);
            // UnBind Vertix array
            glBindVertexArray(0);
        }

        // this function should delete the vertex & element buffers and the vertex array object
        ~Mesh(){
            //TODO: (Req 1) Write this function
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
        }

        Mesh(Mesh const &) = delete;
        Mesh &operator=(Mesh const &) = delete;
    };

}