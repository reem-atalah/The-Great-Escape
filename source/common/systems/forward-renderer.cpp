#include "forward-renderer.hpp"
#include "../mesh/mesh-utils.hpp"
#include "../texture/texture-utils.hpp"

namespace our {

    void ForwardRenderer::initialize(glm::ivec2 windowSize, const nlohmann::json& config){
        // First, we store the window size for later use
        this->windowSize = windowSize;
        enum class LightType {
        DIRECTIONAL,
        POINT,
        SPOT
        };

        // Then we check if there is a sky texture in the configuration
        if(config.contains("sky")){
            // First, we create a sphere which will be used to draw the sky
            this->skySphere = mesh_utils::sphere(glm::ivec2(16, 16));
            
            // We can draw the sky using the same shader used to draw textured objects
            ShaderProgram* skyShader = new ShaderProgram();
            skyShader->attach("assets/shaders/textured.vert", GL_VERTEX_SHADER);
            skyShader->attach("assets/shaders/textured.frag", GL_FRAGMENT_SHADER);
            skyShader->link();
            
            //TODO: (Req 9) Pick the correct pipeline state to draw the sky
            // Hints: the sky will be draw after the opaque objects so we would need depth testing but which depth funtion should we pick?
            // We will draw the sphere from the inside, so what options should we pick for the face culling.
            PipelineState skyPipelineState{};
           
           // enable depth testing to be used
            skyPipelineState.depthTesting.enabled=true;
            // skyPipelineState.depthTesting.function=GL_LEQUAL;//default
            
            // enable face Culling to be used
            skyPipelineState.faceCulling.enabled=true;
            skyPipelineState.faceCulling.culledFace=GL_FRONT; 
            // skyPipelineState.faceCulling.frontFace =GL_CCW; //default 
            //after removing front, it's arleady moves in counter clock wise
            
            
            // Load the sky texture (note that we don't need mipmaps since we want to avoid any unnecessary blurring while rendering the sky)
            std::string skyTextureFile = config.value<std::string>("sky", "");
            Texture2D* skyTexture = texture_utils::loadImage(skyTextureFile, false);

            // Setup a sampler for the sky 
            Sampler* skySampler = new Sampler();
            skySampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_WRAP_S, GL_REPEAT);
            skySampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Combine all the aforementioned objects (except the mesh) into a material 
            this->skyMaterial = new TexturedMaterial();
            this->skyMaterial->shader = skyShader;
            this->skyMaterial->texture = skyTexture;
            this->skyMaterial->sampler = skySampler;
            this->skyMaterial->pipelineState = skyPipelineState;
            this->skyMaterial->tint = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            this->skyMaterial->alphaThreshold = 1.0f;
            this->skyMaterial->transparent = false;
        }

        // Then we check if there is a postprocessing shader in the configuration
        if(config.contains("postprocess")){
            //TODO: (Req 10) Create a framebuffer
            glGenFramebuffers(1, &postprocessFrameBuffer);
            

            //TODO: (Req 10) Create a color and a depth texture and attach them to the framebuffer
            // Hints: The color format can be (Red, Green, Blue and Alpha components with 8 bits for each channel).
            // The depth format can be (Depth component with 24 bits).

            
            // GL_RGBA8 or GL_DEPTH_COMPONENT24 is the formate
            // windowSize the size that has the effect
            colorTarget = texture_utils::empty(GL_RGBA8, windowSize);
            depthTarget= texture_utils::empty(GL_DEPTH_COMPONENT24, windowSize);

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postprocessFrameBuffer);
            // attach to Frame Buffer
            //GLenum target-> GL_DRAW_FRAMEBUFFER 
            //GLenum attachment-> GL_COLOR_ATTACHMENT0  attach the coloe or the Depth GL_DEPTH_ATTACHMENT
            // GLenum textarget -> 2D texture GL_TEXTURE_2D
            // GLuint texture -> the name for the teture
            // GLint level-> mast be 0
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTarget->getOpenGLName(), 0);
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTarget->getOpenGLName(), 0);
            
            //TODO: (Req 10) Unbind the framebuffer just to be safe
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); 
            
            // Create a vertex array to use for drawing the texture
            glGenVertexArrays(1, &postProcessVertexArray);

            // Create a sampler to use for sampling the scene texture in the post processing shader
            Sampler* postprocessSampler = new Sampler();
            postprocessSampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            postprocessSampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Create the post processing shader
            ShaderProgram* postprocessShader = new ShaderProgram();
            postprocessShader->attach("assets/shaders/fullscreen.vert", GL_VERTEX_SHADER);
            postprocessShader->attach(config.value<std::string>("postprocess", ""), GL_FRAGMENT_SHADER);
            postprocessShader->link();

            // Create a post processing material
            postprocessMaterial = new TexturedMaterial();
            postprocessMaterial->shader = postprocessShader;
            postprocessMaterial->texture = colorTarget;
            postprocessMaterial->sampler = postprocessSampler;
            // The default options are fine but we don't need to interact with the depth buffer
            // so it is more performant to disable the depth mask
            postprocessMaterial->pipelineState.depthMask = false;
        }
    }

    void ForwardRenderer::destroy(){
        // Delete all objects related to the sky
        if(skyMaterial){
            delete skySphere;
            delete skyMaterial->shader;
            delete skyMaterial->texture;
            delete skyMaterial->sampler;
            delete skyMaterial;
        }
        // Delete all objects related to post processing
        if(postprocessMaterial){
            glDeleteFramebuffers(1, &postprocessFrameBuffer);
            glDeleteVertexArrays(1, &postProcessVertexArray);
            delete colorTarget;
            delete depthTarget;
            delete postprocessMaterial->sampler;
            delete postprocessMaterial->shader;
            delete postprocessMaterial;
        }
    }

    void ForwardRenderer::render(World* world){
        // First of all, we search for a camera and for all the mesh renderers
        CameraComponent* camera = nullptr;
        //LightComponent *light=nullptr;
        opaqueCommands.clear();
        transparentCommands.clear();
        lightsCommands.clear();
        for(auto entity : world->getEntities()){
            // If we hadn't found a camera yet, we look for a camera in this entity
            if(!camera) camera = entity->getComponent<CameraComponent>();
            // If this entity has a mesh renderer component
            if(auto meshRenderer = entity->getComponent<MeshRendererComponent>(); meshRenderer){
                // We construct a command from it
                RenderCommand command;
                command.localToWorld = meshRenderer->getOwner()->getLocalToWorldMatrix();
                command.center = glm::vec3(command.localToWorld * glm::vec4(0, 0, 0, 1));
                command.mesh = meshRenderer->mesh;
                command.material = meshRenderer->material;
                // if it is transparent, we add it to the transparent commands list
                if(command.material->transparent){
                    transparentCommands.push_back(command);
                } else {
                // Otherwise, we add it to the opaque command list
                    opaqueCommands.push_back(command);
                }
            }

            if(auto lightRenderer = entity->getComponent<LightComponent>(); lightRenderer){
                // We construct a command from LightCommand
                LightCommand command;
                command.localToWorld=lightRenderer->getOwner()->getLocalToWorldMatrix();
                command.lightType = lightRenderer->lightType;
                command.attenuation = lightRenderer->attenuation;
                command.cone_angles = lightRenderer->cone_angles;
                command.diffuse = lightRenderer->diffuse;
                command.specular = lightRenderer->specular;
                command.direction=glm::vec3(command.localToWorld * glm::vec4(-1, 0, 0, 1));
                command.position= glm::vec3(command.localToWorld* glm::vec4 (glm::vec3(2,0,0), 1)); 
                 // add the light to light command list
                lightsCommands.push_back(command);
            }
        }

        // If there is no camera, we return (we cannot render without a camera)
        if(camera == nullptr) return;

        //TODO: (Req 8) Modify the following line such that "cameraForward" contains a vector pointing the camera forward direction
        // HINT: See how you wrote the CameraComponent::getViewMatrix, it should help you solve this one
        // you need M which is model matrix in MVP to change from local to world coordinates
        // then you multiply it by the Mcamera.
        // inorder to folmulate camera you pass eye, center,up direction
        // so getting the camera forward is simply center -eye 
        // normalize it so you are only interested in the direction to be used later->I'm in the later, I need camera position in req9 reem
        auto owner = camera->getOwner();
        auto M = owner->getLocalToWorldMatrix();
        glm::vec3 cameraForward= glm::normalize((glm::vec3(M* glm::vec4 (glm::vec3(0,0,-1), 1))) - (glm::vec3(M* glm::vec4 ( glm::vec3(0,0,0),1))));


        std::sort(transparentCommands.begin(), transparentCommands.end(), [cameraForward] (const RenderCommand& first, const RenderCommand& second){
            //TODO: (Req 8) Finish this function
            // HINT: the following return should return true "first" should be drawn before "second". 

            // here we provide the comparison criteria in the sort function
            // which is just a boolean expression telling if the first is locates further than the second or not
            // if the distance between the first furthest from the camera than the second we return true
            // so the furthest could be drawn first (as a criteria fro correct rendering)
            if(glm::dot(first.center,cameraForward) > glm::dot(second.center,cameraForward)){
                return true;
            }
            return false;
        });

        //TODO: (Req 8) Get the camera ViewProjection matrix and store it in VP  -- not sure
        // just formulating the view projection matrix (remember the right most matrix is multiplied first )
        glm::mat4 VP = camera->getProjectionMatrix(windowSize)*camera->getViewMatrix();
        // glm::mat4 VP = glm::mat4(1.0f);
        //TODO: (Req 8) Set the OpenGL viewport using windowSize
        // passing the dimension of drawing on the view port which is basically here the window dimensions 
        glViewport(0,0,windowSize.x,windowSize.y);  //check on the parameters passed!


        //TODO: (Req 8) Set the clear color to black and the clear depth to 1
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearDepth(1.0);

        //TODO: (Req 8) Set the color mask to true and the depth mask to true (to ensure the glClear will affect the framebuffer)
        glColorMask(true, true, true, true);
        glDepthMask(true);


        // If there is a postprocess material, bind the framebuffer
        if(postprocessMaterial){
            //TODO: (Req 10) bind the framebuffer
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postprocessFrameBuffer);

        }

        //TODO: (Req 8) Clear the color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //TODO: (Req 8) Draw all the opaque commands
        // Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command
        // drawing each opaque pixel by pixel
        // by setting up all the requirements such as 
        // binding to the program created , setting the textures , culling and all other properties needed 
        // then we pass the MVP to the shader 
        // and draw it
         
        //M*eye, M->model matrix transformation, eye->where camera looks
        //M*eye gives the camera position
        for (auto& opaqueCommand : opaqueCommands){
            opaqueCommand.material->setup();
            glm::mat4 M_I =glm::transpose(glm::inverse(opaqueCommand.localToWorld));
            opaqueCommand.material->shader->set("eye",glm::vec3(M* glm::vec4 ( glm::vec3(0,0,0),1)));
            opaqueCommand.material->shader->set("M",opaqueCommand.localToWorld);
            opaqueCommand.material->shader->set("M_IT",M_I);
            opaqueCommand.material->shader->set("transform",VP*opaqueCommand.localToWorld);
            opaqueCommand.material->shader->set("light_count",(int)lightsCommands.size());
            //add the sky light
            opaqueCommand.material->shader->set("sky.top",glm::vec3(0.3, 0.6, 1.0));
            opaqueCommand.material->shader->set("sky.middle",glm::vec3(0.3, 0.3, 0.3));
            opaqueCommand.material->shader->set("sky.bottom",glm::vec3(0.1, 0.1, 0.0));

            int i=0;
            // llop over all lights
            for (auto& lightCommand : lightsCommands){
                //printf("in light");
                //set the type of the light
                std::string Ltype="lights["+std::to_string(i)+"].type";
                if(lightCommand.lightType==LightType::DIRECTIONAL)
                    opaqueCommand.material->shader->set(Ltype,1);
                else if(lightCommand.lightType==LightType::POINT)
                    opaqueCommand.material->shader->set(Ltype,2);
                else if(lightCommand.lightType==LightType::SPOT)
                    opaqueCommand.material->shader->set(Ltype,3);  

                std::string Lposition="lights["+std::to_string(i)+"].position";
                opaqueCommand.material->shader->set(Lposition,lightCommand.position);

                std::string Ldirection="lights["+std::to_string(i)+"].direction";
                opaqueCommand.material->shader->set(Ldirection,lightCommand.direction);  

                std::string Ldiffuse="lights["+std::to_string(i)+"].diffuse";
                opaqueCommand.material->shader->set(Ldiffuse,lightCommand.diffuse);

                std::string Lspecular="lights["+std::to_string(i)+"].specular";
                opaqueCommand.material->shader->set(Lspecular,lightCommand.specular); 

                std::string Lattenuation="lights["+std::to_string(i)+"].attenuation";
                opaqueCommand.material->shader->set(Lattenuation,lightCommand.attenuation);

                std::string Lcone_angles="lights["+std::to_string(i)+"].cone_angles";
                opaqueCommand.material->shader->set(Lcone_angles,lightCommand.cone_angles);

                i++;
            }
            
            opaqueCommand.mesh->draw();
        }
    

        
        // If there is a sky material, draw the sky
        if(this->skyMaterial){
            //TODO: (Req 9) setup the sky material
            this->skyMaterial->setup();
            //TODO: (Req 9) Get the camera position
            glm::vec4 cameraPosition= M* glm::vec4 (glm::vec3(0,0,0), 1); 
            //M*eye, M->model matrix transformation, eye->where camera looks
            //M*eye gives the camera position

            //TODO: (Req 9) Create a model matrix for the sky such that it always follows the camera (sky sphere center = camera position)

            // transform the sky to the position of the camera where sky sphere center = camera position
            Transform skyModel;
            skyModel.position=cameraPosition;
            glm::mat4 skyModelMat = skyModel.toMat4(); //used in next req

            //TODO: (Req 9) We want the sky to be drawn behind everything (in NDC space, z=1)
            // We can acheive this by multiplying by an extra matrix after the projection but what values should we put in it?

            //make z=1 
            // 1. don't multiply z coeff -> 0
            // 2. take value of w to be z , and at normalization it will be 1 where it divides by w
            glm::mat4 alwaysBehindTransform = glm::mat4(
            //  Row1, Row2, Row3, Row4
            //               z
                1.0f, 0.0f, 0.0f, 0.0f, // Column1
                0.0f, 1.0f, 0.0f, 0.0f, // Column2
                0.0f, 0.0f, 0.0f, 0.0f, // Column3 we don't want the value of z
                0.0f, 0.0f, 1.0f, 1.0f  // Column4 we want the value of w 
            );
            //TODO: (Req 9) set the "transform" uniform
            //apply the transformation of sky to follow the camera * camera ViewProjection matrix * matrix making z=1
            this->skyMaterial->shader->set("transform", alwaysBehindTransform * VP * skyModelMat);

            //TODO: (Req 9) draw the sky sphere
            this->skySphere->draw();
        }
        //TODO: (Req 8) Draw all the transparent commands
        // Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command
         for (auto& transparentCommand : transparentCommands){
            transparentCommand.material->setup();
            transparentCommand.material->shader->set("transform",VP*transparentCommand.localToWorld);
            transparentCommand.mesh->draw();
        }

        

        // If there is a postprocess material, apply postprocessing
        if(postprocessMaterial){
            //TODO: (Req 10) Return to the default framebuffer
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            
            //TODO: (Req 10) Setup the postprocess material and draw the fullscreen triangle
            postprocessMaterial->setup();
            glBindVertexArray(postProcessVertexArray);
            //draw the fullscreen triangle
            glDrawArrays(GL_TRIANGLES,0,3);
            
            
        }
    }

}