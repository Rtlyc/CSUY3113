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
#include <SDL_mixer.h>

#include "Entity.h"
#include "Map.h"

class Bullet;
class Monster;

class Tower:public Entity{
public:
    Tower(int index, float size, float range, float damage, float cool_down, Map* map);
    void Update(float delta_time, std::vector<Monster*>& monsters, std::vector<Bullet*>& bullets);
    void Render(ShaderProgram* program);
public:
    float cool_down;
    float timer;
    float range;
    float index;
    float damage;
    GLuint textureID;
    Mix_Chunk* bounce;
};


