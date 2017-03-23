#pragma once

#include <GLFW/glfw3.h>

class InputManager
{
    public:
        // Constructor.
        // GLFWwindow Pointer to GLFWwindow.
        InputManager(GLFWwindow* pGLFWwindow);

        // Destructor.
        ~InputManager();

        // Check if key is pressed.
        // Return whether key is pressed.
        bool KeyPressed(int key);

        // Check if mouse is inside window.
        // Return whether mouse is inside window.
        bool GetMouseInsideWindow();

        // Check if mouse button is pressed.
        // Return whether mouse button is pressed.
        bool GetMouseButtonPressed(int key);

        // Get current mouse position.
        // x Mouse X position.
        // y Mouse Y position.
        void GetMousePositionCurrent(int& x, int& y);

        // Get last mouse position.
        // x Mouse X position.
        // y Mouse Y position.
        void GetMousePositionLast(int& x, int& y);

        // Set mouse position.
        // x Mouse X position.
        // y Mouse Y position.
        void SetMousePosition(int x, int y);

    private:
        GLFWwindow* mpGLFWwindow;
        int mLastMouseX;
        int mLastMouseY;
};
