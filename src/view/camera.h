#pragma once
#include <vec4.hpp>
#include <mat4x4.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include<vector>
#include<deque>
#include<unordered_map>
#include<string>
#include<set>

#ifndef CAMERA_HEADER
#define CAMERA_HEADER
extern double scrollOffset;
extern bool isScrollChanged;
extern bool isMouseLeftBtnPressed;
extern bool isMouseMiddleBtnPressed;
#endif // !CAMERA_HEADER

class Camera {
public:
	glm::vec3 pos;
	glm::vec3 target;
	glm::vec3 front;
	glm::vec3 up;
	float fov;
	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = -1.0f; // Time of last frame
	float getDeltaTime(float currentFrame);
};