#pragma once
#include "../config.h"
#include "logging.h"

class Timing{
public:
    double lastTime{-1}, currentTime;
	int numFrames;
	int lastFrameAccumulate;
	float frameTime;
};