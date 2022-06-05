#pragma once

#include "../ecs/world.hpp"
#include "../components/camera.hpp"
#include "../components/free-camera-controller.hpp"
#include "../components/mesh-renderer.hpp"


#include "../application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/intersect.hpp>

namespace our
{

    // The free camera controller system is responsible for moving every entity which contains a FreeCameraControllerComponent.
    // This system is added as a slightly complex example for how use the ECS framework to implement logic. 
    // For more information, see "common/components/free-camera-controller.hpp"
    class FreeCameraControllerSystem {
        Application* app; // The application in which the state runs
        bool mouse_locked = false; // Is the mouse locked

    public:
        // When a state enters, it should call this function and give it the pointer to the application
        void enter(Application* app){
            this->app = app;
        }

    
        // This should be called every frame to update all entities containing a FreeCameraControllerComponent 
        void update(World* world, float deltaTime) {
            // First of all, we search for an entity containing both a CameraComponent and a FreeCameraControllerComponent
            // As soon as we find one, we break
            CameraComponent* camera = nullptr;
            FreeCameraControllerComponent *controller = nullptr;
            for(auto entity : world->getEntities()){
                camera = entity->getComponent<CameraComponent>();
                controller = entity->getComponent<FreeCameraControllerComponent>();
                if(camera && controller) break;
            }
            // If there is no entity with both a CameraComponent and a FreeCameraControllerComponent, we can do nothing so we return
            if(!(camera && controller)) return;
            // Get the entity that we found via getOwner of camera (we could use controller->getOwner())
            Entity* entity = camera->getOwner();

            // If the left mouse button is pressed, we lock and hide the mouse. This common in First Person Games.
            if(app->getMouse().isPressed(GLFW_MOUSE_BUTTON_1) && !mouse_locked){
                app->getMouse().lockMouse(app->getWindow());
                mouse_locked = true;
            // If the left mouse button is released, we unlock and unhide the mouse.
            } else if(!app->getMouse().isPressed(GLFW_MOUSE_BUTTON_1) && mouse_locked) {
                app->getMouse().unlockMouse(app->getWindow());
                mouse_locked = false;
            }

            // We get a reference to the entity's position and rotation
            glm::vec3& position = entity->localTransform.position;
            glm::vec3& rotation = entity->localTransform.rotation;
            
            glm::vec3& scale = entity->localTransform.scale;





            // If the left mouse button is pressed, we get the change in the mouse location
            // and use it to update the camera rotation
            if(app->getMouse().isPressed(GLFW_MOUSE_BUTTON_1)){
                glm::vec2 delta = app->getMouse().getMouseDelta();
                rotation.x -= delta.y * controller->rotationSensitivity; // The y-axis controls the pitch
                rotation.y -= delta.x * controller->rotationSensitivity; // The x-axis controls the yaw
            }

            // We prevent the pitch from exceeding a certain angle from the XZ plane to prevent gimbal locks
            if(rotation.x < -glm::half_pi<float>() * 0.99f) rotation.x = -glm::half_pi<float>() * 0.99f;
            if(rotation.x >  glm::half_pi<float>() * 0.99f) rotation.x  = glm::half_pi<float>() * 0.99f;
            // This is not necessary, but whenever the rotation goes outside the 0 to 2*PI range, we wrap it back inside.
            // This could prevent floating point error if the player rotates in single direction for an extremely long time. 
            rotation.y = glm::wrapAngle(rotation.y);

            // We update the camera fov based on the mouse wheel scrolling amount
            float fov = camera->fovY + app->getMouse().getScrollOffset().y * controller->fovSensitivity;
            fov = glm::clamp(fov, glm::pi<float>() * 0.01f, glm::pi<float>() * 0.99f); // We keep the fov in the range 0.01*PI to 0.99*PI
            camera->fovY = fov;

            // We get the camera model matrix (relative to its parent) to compute the front, up and right directions
            glm::mat4 matrix = entity->localTransform.toMat4();

            glm::vec3 front = glm::vec3(matrix * glm::vec4(0, 0, -1, 0)),
                      up = glm::vec3(matrix * glm::vec4(0, 1, 0, 0)), 
                      right = glm::vec3(matrix * glm::vec4(1, 0, 0, 0));

            glm::vec3 current_sensitivity = controller->positionSensitivity;
            // If the LEFT SHIFT key is pressed, we multiply the position sensitivity by the speed up factor
            if(app->getKeyboard().isPressed(GLFW_KEY_LEFT_SHIFT)) current_sensitivity *= controller->speedupFactor;




            // To Detect Collision
            // 1- Get verticies  & elements of Maze in world space
            MeshRendererComponent * mazeComponent;
            for(auto entity : world->getEntities()){

                mazeComponent = entity->getComponent<MeshRendererComponent>();
                if(mazeComponent && mazeComponent->isMaze) break;
            }

            if(!mazeComponent) return;

            std::vector<Vertex> mazeVerticies_local=mazeComponent->mesh->mesh_vertices;
            std::vector<GLuint> mazeElements =mazeComponent->mesh->mesh_elements;
            glm::mat4 mazeModelMatrix=mazeComponent->getOwner()->getLocalToWorldMatrix();

            // 2- Save old camera position in  World Space
            glm::vec3 newPosition=position; 

            // 3- Move Camera according to direction   to get new position of camera in World 
             // We change the camera position based on the keys WASD/QE
            // S & W moves the player back and forth
            // Q & E moves the player up and down
            // A & D moves the player left or right
            if(app->getKeyboard().isPressed(GLFW_KEY_W)) newPosition += front * (deltaTime * current_sensitivity.z);
            if(app->getKeyboard().isPressed(GLFW_KEY_S)) newPosition -= front * (deltaTime * current_sensitivity.z);
            if(app->getKeyboard().isPressed(GLFW_KEY_D)) newPosition += right * (deltaTime * current_sensitivity.x);
            if(app->getKeyboard().isPressed(GLFW_KEY_A)) newPosition -= right * (deltaTime * current_sensitivity.x);
            if(app->getKeyboard().isPressed(GLFW_KEY_Q)) newPosition += up    * (deltaTime * current_sensitivity.y);
            if(app->getKeyboard().isPressed(GLFW_KEY_E)) newPosition -= up    * (deltaTime * current_sensitivity.y);

            // 4- get  direction of movement
            bool colloided=false;
            glm::vec3 movementDirection =glm::normalize(newPosition - position);

            size_t element_count = (mazeElements.size() / 3) * 3;
            for(size_t element = 0; element < element_count;){
                glm::vec3 v0 = mazeModelMatrix * glm::vec4(mazeVerticies_local[mazeElements[element++]].position, 1.0f);
                glm::vec3 v1 = mazeModelMatrix * glm::vec4(mazeVerticies_local[mazeElements[element++]].position, 1.0f);
                glm::vec3 v2 = mazeModelMatrix * glm::vec4(mazeVerticies_local[mazeElements[element++]].position, 1.0f);
                glm::vec2 barycentric_coords; float triangle_hit_distance;

                if(glm::intersectRayTriangle(position, movementDirection, v0, v1, v2, barycentric_coords, triangle_hit_distance)){
                    if(triangle_hit_distance>0 && triangle_hit_distance<= glm::distance(position,newPosition)){
                            colloided=true;
                            break;
                    }                    
                }
            }
            // 5- Check condition of collision
            if(!colloided) {position=newPosition;}

  
        }

        // When the state exits, it should call this function to ensure the mouse is unlocked
        void exit(){
            if(mouse_locked) {
                mouse_locked = false;
                app->getMouse().unlockMouse(app->getWindow());
            }
        }

    };

}
