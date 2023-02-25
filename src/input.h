#ifndef INPUT_H
#define INPUT_H

#include "Source.h"
#include "cameraMovement.h"

//mouse input
void mouse_callback(GLFWwindow* window, double xposInput, double yposInput)
{
    float xpos = static_cast<float>(xposInput);
    float ypos = static_cast<float>(yposInput);

    if (firstMouse){
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset, yoffset;

    if (!paused) {
        xoffset = xpos - lastX;
        yoffset = lastY - ypos;
    }
    else {
        xoffset = 0;
        yoffset = 0;
    }

    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

//mouse scroll whell input
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

}

//keyboard input
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == 1) {
        paused = !paused;
    }
}

//input (called every frame)
void processInput(GLFWwindow* window)
{
    if (!paused) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            cameraSpeed = glm::mix(cameraSpeed, (runSpeed / 2) * movementMultiplier, runSmoothing / 10);
        }
        else {
            cameraSpeed = glm::mix(cameraSpeed, (walkSpeed / 2) * movementMultiplier, runSmoothing / 10);
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            desiredPos += (cameraSpeed)*glm::normalize(glm::vec3(direction.x, 0.0f, direction.z));
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            desiredPos -= (cameraSpeed)*glm::normalize(glm::vec3(direction.x, 0.0f, direction.z));
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            desiredPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * (cameraSpeed);
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            desiredPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * (cameraSpeed);
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            if (!jump) {
                jump = true;
                velocityY += jumpHeight;
            }
        }

        //camera smoothing
        cameraPos.x = glm::mix(cameraPos.x, desiredPos.x, smoothing / 100);
        cameraPos.z = glm::mix(cameraPos.z, desiredPos.z, smoothing / 100);

        cameraPos.y += velocityY * 0.01f;

        if (cameraPos.y > terrainHeight * 0.02f)
            velocityY += gravity * 0.01f;
        else {
            jump = false;
            velocityY = 0.0f;
        }
    }
}

//mouse button input
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{

}

//window refresh
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

#endif