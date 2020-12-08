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

class Bullet;
class Monster;

class Monster:public Entity{
public:
    Monster(float x, float speed, float size, float maxhealth);
    
    Monster(float x, float y, float size, float speed, float maxhealth);
    
    void Destroy(std::vector<Monster*>& monsters);
    
    void Update(float delta_time, std::vector<Monster*>& monsters, std::vector<Bullet*>& bullets, int& player_health, int& money);
    
    void Render(ShaderProgram* program);
public:
    float cur_health=1;
    float health=1;
    float speed;
};

