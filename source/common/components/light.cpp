#include "light.hpp"
#include "../ecs/entity.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include "../deserialize-utils.hpp"

namespace our {
    // Reads Light parameters from the given json object
    void LightComponent::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;
        std::string lightTypeStr = data.value("lightType", "directional");
        if(lightTypeStr == "directional"){
            lightType = LightType::DIRECTIONAL;
        } else if(lightTypeStr == "point"){
            lightType = LightType::POINT;
        }else{
            lightType = LightType::SPOT;
        }
        diffuse = glm::vec3(data.value("diffuse", glm::vec3(1, 0.9, 1)));
        specular = glm::vec3(data.value("specular", glm::vec3(1, 0.9, 1)));
        attenuation = glm::vec3(data.value("attenuation", glm::vec3(0, 0, 0)));
        cone_angles = glm::vec2(data.value("cone_angles", glm::vec2(glm::radians(10.0f), glm::radians(11.0f))));
    }

    
}