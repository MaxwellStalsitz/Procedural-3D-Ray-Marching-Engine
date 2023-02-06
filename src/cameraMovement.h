#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/ext.hpp>

//delta time variables
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float walkSpeed = 0.3f;
float runSpeed = 0.45f;
float movementMultiplier = 1.0f;

bool jump = false;
float jumpHeight = 12.5f;

glm::vec3 lastDesiredPos;

float smoothing = 10.0f;
float runSmoothing = 15.0f;

float velocityY = 0.0f;
const float gravity = -50.f;

//default height set for player
float terrainHeight = 0.0f;

//camera variables for orientation
glm::vec3 cameraPos = glm::vec3(0.0f, terrainHeight, 0.0f);
glm::vec3 desiredPos = cameraPos;
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//camera speed variable
float cameraSpeed = walkSpeed;

//mouse/camera variables
bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 75.0f;