//----------------------------------------------------------------------------------------
/**
 * \file       LightSourses.h
 * \author     Plotnikau Pavel
 * \date       2021/05/20
 * \brief      Defines different types of light sources
*/
//----------------------------------------------------------------------------------------
#ifndef LIGHT_SOURCES
#define LIGHT_SOURCES

#include "pgr.h"

/// <summary>
/// Defines information of direct light sourse
/// </summary>
struct DirectLight {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float intensity;
    glm::vec3 direction;
};

/// <summary>
/// Defines information of point light sourse
/// </summary>
struct PointLight {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float intensity;
    glm::vec3 position;
    float linear;
    float quadratic;
};

/// <summary>
/// Defines information of spot light sourse
/// </summary>
struct SpotLight {
    PointLight point;

    glm::vec3 direction;
    float cut_off;
};

#endif
