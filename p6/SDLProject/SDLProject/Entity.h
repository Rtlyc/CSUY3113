#pragma once

#define GL_SILENCE_DEPRECATION
#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <vector>
#include <math.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

class Entity{
public:
    Entity(float x, float y, float size):x(x),y(y),size(size){}
    
    Entity(float size): size(size){}
    
    bool CheckCollision(Entity* other);
public:
    float x,y;
    float size;
    float drawArray[12] = { -0.5f*size, 0.5f*size, 0.5f*size, 0.5f*size, 0.5f*size, -0.5f*size, 0.5f*size, -0.5f*size, -0.5f*size, -0.5f*size, -0.5f*size, 0.5f*size };
};


