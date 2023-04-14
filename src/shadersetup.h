#pragma once

#include "Source.h"
#include "Shader.h"
#include "cameraMovement.h"

void setupRayMarching(Shader * rayMarchingShader){

    rayMarchingShader->setVec2("resolution", glm::vec2(screenWidth * 1.2857, screenHeight));
    rayMarchingShader->setFloat("time", glfwGetTime());
    rayMarchingShader->setFloat("deltaTime", deltaTime);
    rayMarchingShader->setVec2("mousePosition", glm::vec2(xpo, ypo));

    rayMarchingShader->setVec3("cameraPos", cameraPos);
    rayMarchingShader->setVec3("direction", direction);
    rayMarchingShader->setVec3("cameraFront", cameraFront);

    rayMarchingShader->setBool("useLighting", useLighting);
    rayMarchingShader->setVec3("lightPosition", lightPosition);

    rayMarchingShader->setInt("MAX_STEPS", MAX_STEPS);
    rayMarchingShader->setFloat("MAX_DIST", MAX_DIST);
    rayMarchingShader->setFloat("MIN_DIST", MIN_DIST);

    rayMarchingShader->setBool("antiAliasing", antiAliasing);

    rayMarchingShader->setInt("scene", scene);

    rayMarchingShader->setBool("ambientOcclusion", ambientOcclusion);
    rayMarchingShader->setInt("samples", occlusionSamples);

    rayMarchingShader->setFloat("power", power);
    rayMarchingShader->setInt("iterations", iterations);
    rayMarchingShader->setBool("animate", animate);
    rayMarchingShader->setFloat("timeMultiplier", timeMultiplier);

    rayMarchingShader->setInt("numberOfObjects", numberOfEntities);

    rayMarchingShader->setBool("reflections", reflections);
    rayMarchingShader->setFloat("reflectionVisibility", reflectionVisibility);

    rayMarchingShader->setBool("fogEnabled", fogEnabled);
    rayMarchingShader->setFloat("fogVisibility", fogVisibility);
    rayMarchingShader->setFloat("falloff", falloff);

    rayMarchingShader->setFloat("smoothness", smoothness);

    //sending array of object values to shader for scene editor
    //there is probably a better solution to this

    const char* name;

    for (int i = 0; i < numberOfEntities; ++i)
    {
        std::stringstream ss;
        ss << "objectPositions[" << i << "]";
        std::string str = ss.str();
        name = str.c_str();

        rayMarchingShader->setVec3(name, sceneArray[i].position);
        ss.str("");

        ss << "objectScale[" << i << "]";
        str = ss.str();
        name = str.c_str();

        rayMarchingShader->setVec3(name, sceneArray[i].scale);
        ss.str("");

        ss << "primitives[" << i << "]";
        str = ss.str();
        name = str.c_str();

        rayMarchingShader->setInt(name, sceneArray[i].shape);
        ss.str("");

        ss << "objectColors[" << i << "]";
        str = ss.str();
        name = str.c_str();

        rayMarchingShader->setVec3(name, sceneArray[i].color);
        ss.str("");

        ss << "objectRotations[" << i << "]";
        str = ss.str();
        name = str.c_str();

        rayMarchingShader->setVec3(name, sceneArray[i].rotation);
        ss.str("");
    }

    rayMarchingShader->setInt("primitive", primitiveSelected);
}

void setupRayTracing(Shader * rayTracingShader){
    //ray tracing variables

    rayTracingShader->setVec2("resolution", glm::vec2(screenWidth, screenHeight));
    rayTracingShader->setVec2("mouse", glm::vec2(xpo, ypo));
    rayTracingShader->setFloat("time", glfwGetTime());

    rayTracingShader->setInt("scene", scene);

    rayTracingShader->setVec3("cameraPos", cameraPos);
    rayTracingShader->setVec3("direction", direction);
    rayTracingShader->setVec3("cameraFront", cameraFront);

    rayTracingShader->setBool("fogEnabled", fogEnabled);
    rayTracingShader->setFloat("fogVisibility", fogVisibility);
    rayTracingShader->setFloat("falloff", falloff);

    rayTracingShader->setBool("useLighting", useLighting);
    rayTracingShader->setVec3("lightPosition", lightPosition);

    rayTracingShader->setBool("reflections", reflections);
    rayTracingShader->setFloat("visibility", reflectionVisibility);

    rayTracingShader->setBool("antiAliasing", antiAliasing);
    rayTracingShader->setBool("ambientOcclusion", ambientOcclusion);
}