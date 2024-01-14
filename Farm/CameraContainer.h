//----------------------------------------------------------------------------------------
/**
 * \file       CameraContainer.h
 * \author     Plotnikau Pavel
 * \date       2021/05/20
 * \brief      Defines the basic parameters related to the position and direction of the camera 
*/
//----------------------------------------------------------------------------------------
#ifndef CAMERA_CONTAINER
#define CAMERA_CONTAINER

#include "pgr.h"

/// <summary>
/// Defines camera position, looking diraction and up-vector
/// </summary>
struct Camera {
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 camera_up;
};

#endif