#pragma once

#include "../ecs/component.hpp"

#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>

namespace our {

    // An enum that defines the type of the light (DIRECTIONAL OR POINT OR SPOT) 
    enum class LightType {
        DIRECTIONAL,
        POINT,
        SPOT
    };


    class LightComponent : public Component {
    public:
        LightType lightType; // The type of the light
        glm::vec3 diffuse;
        glm::vec3 specular;
        glm::vec3 attenuation; // x*d^2 + y*d + z
        glm::vec2 cone_angles; // x: inner_angle, y: outer_angle

        // The ID of this component type is "Light"
        static std::string getID() { return "Light"; }

        // Reads light parameters from the given json object
        void deserialize(const nlohmann::json& data) override;
    };

}