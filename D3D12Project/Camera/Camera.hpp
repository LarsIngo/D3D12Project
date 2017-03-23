#pragma once

#include <glm/glm.hpp>

class FrameBuffer;
class InputManager;

// A camera.
class Camera 
{
    public:
        // Constructor.
        // fov Field of view in degrees.
        // frameBuffer Framebuffer to render.
        Camera(float fov, FrameBuffer* frameBuffer);

        // Destructor.
        ~Camera();

        // Update camera attributes.
        // moveSpeed Speed of movement.
        // rotationSpeed Speed of rotation-
        // dt Delta time.
        // inputManager Input manager to handle inputs. 
        void Update(float moveSpeed, float rotationSpeed, float dt, InputManager* inputManager);

        // Rotate camera around up direction.
        // rotation Amount of rotation in degrees.
        void Yaw(float rotation);

        // Rotate camera around right direction.
        // rotation Amount of rotation in degrees.
        void Pitch(float rotation);

        // Rotate camera around front direction.
        // rotation Amount of rotation in degrees.
        void Roll(float rotation);

        // Position.
        glm::vec3 mPosition = glm::vec3(0.f, 0.f, 0.f);

        // Front.
        glm::vec3 mFrontDirection = glm::vec3(0.f, 0.f, 1.f);

        // Up.
        glm::vec3 mUpDirection = glm::vec3(0.f, 1.f, 0.f);

        // Right.
        glm::vec3 mRightDirection = glm::vec3(1.f, 0.f, 0.f);

        // Orientation matrix.
        glm::mat4 mOrientationMatrix;

        // View matrix.
        glm::mat4 mViewMatrix;

        // Projection matrix.
        glm::mat4 mProjectionMatrix;

        // Frame buffer pointer.
        FrameBuffer* mpFrameBuffer;

        // Field of view in degrees.
        float mFov;

        // Near clip plane.
        float mNearZ;

        // Far clip plane.
        float mFarZ;

    private:
        glm::mat4 CalculateOrientationMatrix() const;
        glm::mat4 CalculateViewMatrix() const;
};
