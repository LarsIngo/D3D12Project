#include "InputManager.hpp"

InputManager::InputManager(GLFWwindow* pGLFWwindow)
{
    mpGLFWwindow = pGLFWwindow;
    mLastMouseX = 0;
    mLastMouseY = 0;
}

InputManager::~InputManager()
{

}

bool InputManager::KeyPressed(int key)
{
    return glfwGetKey(mpGLFWwindow, key);
}


bool InputManager::GetMouseInsideWindow()
{
    int width, height;
    glfwGetWindowSize(mpGLFWwindow, &width, &height);
    double xPos, yPos;
    glfwGetCursorPos(mpGLFWwindow, &xPos, &yPos);
    int x = static_cast<int>(xPos);
    int y = static_cast<int>(yPos);
    return x >= 0 && x < width && y >= 0 && y < height;
}

bool InputManager::GetMouseButtonPressed(int key)
{
    return glfwGetMouseButton(mpGLFWwindow, key) == GLFW_PRESS;
}


void InputManager::GetMousePositionCurrent(int& x, int& y)
{
    double xPos, yPos;
    glfwGetCursorPos(mpGLFWwindow, &xPos, &yPos);
    x = static_cast<int>(xPos);
    y = static_cast<int>(yPos);
    mLastMouseX = x;
    mLastMouseY = y;
}

void InputManager::GetMousePositionLast(int& x, int& y)
{
    x = mLastMouseX;
    y = mLastMouseY;
}

void InputManager::SetMousePosition(int x, int y)
{
    glfwSetCursorPos(mpGLFWwindow, x, y);
}
