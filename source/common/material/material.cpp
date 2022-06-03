#include "material.hpp"

#include "../asset-loader.hpp"
#include "deserialize-utils.hpp"

namespace our {

    // This function should setup the pipeline state and set the shader to be used
    void Material::setup() const {
        //TODO: (Req 6) Write this function
        
        // Using the program that is attached to the 2 shaders fshader and vshader of the current material
        shader->use();

        // Enabling and Setting the OpenGL options with the values specified in the pipeline struct
        pipelineState.setup();
    }

    // This function read the material data from a json object 
    void Material::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;

        if(data.contains("pipelineState")){
            pipelineState.deserialize(data["pipelineState"]);
        }
        shader = AssetLoader<ShaderProgram>::get(data["shader"].get<std::string>());
        transparent = data.value("transparent", false);
    }

    // This function should call the setup of its parent and
    // set the "tint" uniform to the value in the member variable tint 
    void TintedMaterial::setup() const {
        //TODO: (Req 6) Write this function
        
        // Calling the setup of the parent to set the pipeline state and the shader to be used with drawing the object
        Material::setup();

        // Set the tint uniform value in the shader with the member variable value 'tint' read from the json object
        shader->set("tint", tint);
    }

    // This function read the material data from a json object
    void TintedMaterial::deserialize(const nlohmann::json& data){
        Material::deserialize(data);
        if(!data.is_object()) return;
        tint = data.value("tint", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    // This function should call the setup of its parent and
    // set the "alphaThreshold" uniform to the value in the member variable alphaThreshold
    // Then it should bind the texture and sampler to a texture unit and send the unit number to the uniform variable "tex" 
    void TexturedMaterial::setup() const {
        //TODO: (Req 6) Write this function

        // Calling the setup of the parent to set the tint, the pipeline state and the shader to be used with drawing the object
        TintedMaterial::setup();

        // Set the alphaThreshold uniform value in the shader with the member variable value 'alphaThreshold' read from the json object
        shader->set("alphaThreshold", alphaThreshold);

        // ??? Which texture unit to bind on ???
        int textUnitNumber = 0;
        GLenum texUnitEnum = GL_TEXTURE0 + textUnitNumber;
        glActiveTexture(texUnitEnum);
        texture->bind();
        sampler->bind(textUnitNumber);

        shader->set("tex", textUnitNumber);
    }

    // This function read the material data from a json object
    void TexturedMaterial::deserialize(const nlohmann::json& data){
        TintedMaterial::deserialize(data);
        if(!data.is_object()) return;
        alphaThreshold = data.value("alphaThreshold", 0.0f);
        texture = AssetLoader<Texture2D>::get(data.value("texture", ""));
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
    }


    void LitMaterial::setup() const {
        //TODO: (Req 6) Write this function

        // Calling the setup of the parent to set the tint, the pipeline state and the shader to be used with drawing the object
        Material::setup();

        // ??? Which texture unit to bind on ???
        int textUnitNumber = 0;
        GLenum texUnitEnum = GL_TEXTURE0 + textUnitNumber;
        glActiveTexture(texUnitEnum);
        albedo->bind();
        sampler->bind(textUnitNumber);
        shader->set("material.albedo", textUnitNumber);


        textUnitNumber = 1;
        texUnitEnum = GL_TEXTURE0 + textUnitNumber;
        glActiveTexture(texUnitEnum);
        specular->bind();
        sampler->bind(textUnitNumber);
        shader->set("material.specular", textUnitNumber);

        textUnitNumber = 2;
        texUnitEnum = GL_TEXTURE0 + textUnitNumber;
        glActiveTexture(texUnitEnum);
        ambient_occlusion->bind();
        sampler->bind(textUnitNumber);
        shader->set("material.ambient_occlusion", textUnitNumber);

        textUnitNumber = 3;
        texUnitEnum = GL_TEXTURE0 + textUnitNumber;
        glActiveTexture(texUnitEnum);
        roughness->bind();
        sampler->bind(textUnitNumber);
        shader->set("material.roughness", textUnitNumber);

        textUnitNumber = 4;
        texUnitEnum = GL_TEXTURE0 + textUnitNumber;
        glActiveTexture(texUnitEnum);
        emissive->bind();
        sampler->bind(textUnitNumber);
        shader->set("material.emissive", textUnitNumber);
        
    }

        void LitMaterial::deserialize(const nlohmann::json& data){
        Material::deserialize(data);
        if(!data.is_object()) return;
        albedo = AssetLoader<Texture2D>::get(data.value("albedo", ""));
        specular = AssetLoader<Texture2D>::get(data.value("specular", ""));
        ambient_occlusion = AssetLoader<Texture2D>::get(data.value("ambient_occlusion", ""));
        roughness = AssetLoader<Texture2D>::get(data.value("roughness", ""));
        emissive = AssetLoader<Texture2D>::get(data.value("emissive", ""));

        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
        }

}