#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "../D3D12/FrameBuffer.hpp"
#include "../Managers/InputManager.hpp"

Camera::Camera(float fov, FrameBuffer* frameBuffer)
{
    mFov = fov;
    mNearZ = 0.01f;
    mFarZ = 200000.f;
    mpFrameBuffer = frameBuffer;
    mProjectionMatrix = glm::perspectiveFovLH(glm::radians(mFov), (float)mpFrameBuffer->mWidth, (float)mpFrameBuffer->mHeight, mNearZ, mFarZ);
}

Camera::~Camera() 
{

}

void Camera::Update(float moveSpeed, float rotationSpeed, float dt, InputManager* inputManager)
{
    // Get movement & rotation.
    glm::vec3 movement = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 rotation = glm::vec3(0.f, 0.f, 0.f);

    if (inputManager->KeyPressed(GLFW_KEY_W))
    {
        movement += mFrontDirection;
    }
    if (inputManager->KeyPressed(GLFW_KEY_A))
    {
        movement -= mRightDirection;
    }
    if (inputManager->KeyPressed(GLFW_KEY_S))
    {
        movement -= mFrontDirection;
    }
    if (inputManager->KeyPressed(GLFW_KEY_D))
    {
        movement += mRightDirection;
    }

    if (inputManager->KeyPressed(GLFW_KEY_Q))
    {
        movement += mUpDirection;
    }

    if (inputManager->KeyPressed(GLFW_KEY_E))
    {
        movement -= mUpDirection;
    }

    if (inputManager->KeyPressed(GLFW_KEY_X))
    {
        rotation.z -= 360.f;
    }

    if (inputManager->KeyPressed(GLFW_KEY_Z))
    {
        rotation.z += 360.f;
    }

    // Update postion & rotation.
    mPosition += movement * moveSpeed * dt;
    rotation *= rotationSpeed * dt;

    // Update mouse rotation.
    if (inputManager->GetMouseInsideWindow())
    {
        int x, y, lastX, lastY;
        inputManager->GetMousePositionLast(lastX, lastY);
        inputManager->GetMousePositionCurrent(x, y);
        if (inputManager->GetMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
        {
            int dx = lastX - x;
            int dy = lastY - y;
            rotation.x += (float)dx / mpFrameBuffer->mWidth * 2.f * mFov;
            rotation.y += (float)dy / mpFrameBuffer->mHeight * 2.f * mFov * mpFrameBuffer->mHeight / mpFrameBuffer->mWidth;
        }
    }

    // Update direction vectors and matrices.
    Roll(rotation.z);
    Pitch(rotation.y);
    Yaw(rotation.x);

    mOrientationMatrix = CalculateOrientationMatrix();
    mViewMatrix = CalculateViewMatrix();
}

void Camera::Yaw(float rotation)
{
    glm::quat q = glm::angleAxis(glm::radians(rotation), mUpDirection);
    mRightDirection = glm::normalize(q * mRightDirection);
    mFrontDirection = glm::normalize(q * mFrontDirection);
}

void Camera::Pitch(float rotation)
{
    glm::quat q = glm::angleAxis(glm::radians(rotation), mRightDirection);
    mFrontDirection = glm::normalize(q * mFrontDirection);
    mUpDirection = glm::normalize(q * mUpDirection);
}

void Camera::Roll(float rotation)
{
    glm::quat q = glm::angleAxis(glm::radians(rotation), mFrontDirection);
    mUpDirection = glm::normalize(q * mUpDirection);
    mRightDirection = glm::normalize(q * mRightDirection);
}

glm::mat4 Camera::CalculateOrientationMatrix() const
{
    return glm::transpose(glm::mat4(
        glm::vec4(mRightDirection, 0.f),
        glm::vec4(mUpDirection, 0.f),
        glm::vec4(mFrontDirection, 0.f),
        glm::vec4(0.f, 0.f, 0.f, 1.f)
    ));
}

glm::mat4 Camera::CalculateViewMatrix() const 
{
    return mOrientationMatrix * glm::translate(glm::mat4(), -mPosition);
}
