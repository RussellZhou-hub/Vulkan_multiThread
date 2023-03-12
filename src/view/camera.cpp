#include "camera.h"

double scrollOffset=0;
bool isScrollChanged = false;
bool isMouseLeftBtnPressed = false;
bool isMouseMiddleBtnPressed = false;

float Camera::getDeltaTime(float currentFrame){
    if (lastFrame == -1.0f) return 0.01f;
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	return deltaTime;
}