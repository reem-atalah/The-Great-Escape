#pragma once

#include <glad/gl.h>
#include <glm/vec4.hpp>
#include <json/json.hpp>

namespace our {
    // There are some options in the render pipeline that we cannot control via shaders
    // such as blending, depth testing and so on
    // Since each material could require different options (e.g. transparent materials usually use blending),
    // we will encapsulate all these options into a single structure that will also be responsible for configuring OpenGL's pipeline
    struct PipelineState {
        // This set of pipeline options specifies whether face culling will be used or not and how it will be configured
        struct {
            bool enabled = false;
            GLenum culledFace = GL_BACK;
            GLenum frontFace = GL_CCW;
        } faceCulling;

        // This set of pipeline options specifies whether depth testing will be used or not and how it will be configured
        struct {
            bool enabled = false;
            GLenum function = GL_LEQUAL;
        } depthTesting;

        // This set of pipeline options specifies whether blending will be used or not and how it will be configured
        struct {
            bool enabled = false;
            GLenum equation = GL_FUNC_ADD;
            GLenum sourceFactor = GL_SRC_ALPHA;
            GLenum destinationFactor = GL_ONE_MINUS_SRC_ALPHA;
            glm::vec4 constantColor = {0, 0, 0, 0};
        } blending;

        // These options specify the color and depth mask which can be used to
        // prevent the rendering/clearing from modifying certain channels of certain targets in the framebuffer
        glm::bvec4 colorMask = {true, true, true, true}; // To know how to use it, check glColorMask
        bool depthMask = true; // To know how to use it, check glDepthMask


        // This function should set the OpenGL options to the values specified by this structure
        // For example, if faceCulling.enabled is true, you should call glEnable(GL_CULL_FACE), otherwise, you should call glDisable(GL_CULL_FACE)
        void setup() const {
            //TODO: (Req 3) Write this function

            // Checking the status of the Face culling option
            // ----------------------------------------------
            // If the faceculling option was enabled
            // Then configure opengl options by setting them with the faceculling options
            if (faceCulling.enabled) {
                // Enable Face Culling property
                glEnable(GL_CULL_FACE);
                // Specifying the face to be culled
                // The member culledFace of the struct faceCulling holds which face to be culled
                glCullFace(faceCulling.culledFace);
                // Specifying the direction that defines what makes a front face
                // The member frontFace of the struct faceCulling holds which direction CCW or CW the Front face is.
                glFrontFace(faceCulling.frontFace);
            }
            else {
                // Disable the face culling if it wasn't enabled in the pipeline struct.
                glDisable(GL_CULL_FACE);
            }

            // Checking the status of the Depth Testing option
            // ----------------------------------------------
            // If the depth testing option was enabled
            // Then configure opengl options by setting them with the depthTesting options
            if (depthTesting.enabled) {
                // Enable Depth Testing property
                glEnable(GL_DEPTH_TEST);
                // Specifying the comparision function where if it's satisfied,
                // Then the value stored in the depth buffer at this pixel location
                // Will be overwritten (replaced) by the value of the object we're comparing with.
                glDepthFunc(depthTesting.function);
            }
            else {
                // Disable the depth testing if it wasn't enabled in the pipeline struct.
                glDisable(GL_DEPTH_TEST);
            }

            // Checking the status of the Blending option
            // ----------------------------------------------
            // If the blending option was enabled
            // Then configure opengl options by setting them with the blending options
            if (blending.enabled) {
                glEnable(GL_BLEND);
                glBlendEquation(blending.equation);
                glBlendFunc(blending.sourceFactor, blending.destinationFactor);
                glBlendColor(blending.constantColor[0], blending.constantColor[1], blending.constantColor[2], blending.constantColor[3]);
            }
            else {
                // Disable the blending if this option in the pipeline strcut was not enabled.
                glDisable(GL_BLEND);
            }

            // Setting the Color Mask for the 4 channels with either true or false
            // To allow or disallow modifying on these channel.
            glColorMask(colorMask[0], colorMask[1], colorMask[2], colorMask[3]);
            // Setting the Depth Mask either true or false
            // To allow or disallow modifying on the depth buffer.
            glDepthMask(depthMask);

        }

        // Given a json object, this function deserializes a PipelineState structure
        void deserialize(const nlohmann::json& data);
    };

}