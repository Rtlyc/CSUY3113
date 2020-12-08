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

#include "Entity.h"

class Tower;

class Bullet:public Entity{
public:
    Bullet(float x, float y, float size, glm::vec2 direction, float speed, float damage, Tower* parent);
    
    void Update(float delta_time, std::vector<Bullet*>& bullets);
    
    void Render(ShaderProgram* program);
    
    void Destroy(std::vector<Bullet*>& bullets);
public:
    float damage;
    float speed;
    glm::vec2 direction;
    Tower* parent;
};


