#include "entity.hpp"
#include "../deserialize-utils.hpp"

#include <glm/gtx/euler_angles.hpp>
#include <iostream>
using namespace std;

namespace our {

    // This function computes and returns a matrix that represents this transform
    // Remember that the order of transformations is: Scaling, Rotation then Translation
    // HINT: to convert euler angles to a rotation matrix, you can use glm::yawPitchRoll
    glm::mat4 Transform::toMat4() const {
        //TODO: (Req 2) Write this function

        glm::mat4 scaling_matrix=
                                glm::mat4(glm::vec4(scale[0],0,0,0), 
                                glm::vec4(0,scale[1],0,0), 
                                glm::vec4(0,0,scale[2],0) , 
                                glm::vec4(0,0,0,1));

        glm::mat4 rotation_matrix= glm::yawPitchRoll(rotation.y, rotation.x, rotation.z);

        glm::mat4 translation_matrix= glm::mat4(glm::vec4(1,0,0,0), 
                                                glm::vec4(0,1,0,0), 
                                                glm::vec4(0,0,1,0) , 
                                                glm::vec4(position[0],position[1],position[2],1));
        return  translation_matrix * rotation_matrix * scaling_matrix  ;
        // return glm::mat4(1.0f);

        // glm::mat4 transMatrix = glm::translate(
        // glm::mat4( 1.0f ),
        // position
        // );

        // glm::mat4 ScaleMatrix = glm::scale(  
        // glm::mat4( 1.0f ),            
        // scale
        // );

        // return transMatrix * rotation_matrix*  ScaleMatrix ;
    }

     // Deserializes the entity data and components from a json object
    void Transform::deserialize(const nlohmann::json& data){
        position = data.value("position", position);
        rotation = glm::radians(data.value("rotation", glm::degrees(rotation)));
        scale    = data.value("scale", scale);
    }

}