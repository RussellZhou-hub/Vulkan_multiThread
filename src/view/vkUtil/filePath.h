#pragma once
#ifndef UTILS
#define UTILS

#include <vec4.hpp>
#include <mat4x4.hpp>
#include <stdlib.h>
#include <stdio.h>
#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>
#define MAX_PATH 2048
#endif

#include <string>
#include<iostream>


using namespace std;

string getExePath();
string getPath_obj(std::string& exePath,std::string& name);
string getPath_mtl(std::string path);


#endif