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

class Tower;

enum GridType{
    BLANK,
    WHITE,
    GRASS,
    ROAD
};

struct Grid{
    float x,y; //location
    int i,j; //index
    GridType type;
    Tower* tower;
};

class Map{
public:
    Map(unsigned int* data, int width, int height, float x, float y);
    
    void IndexToXY(int index, int& x, int& y);
    
    int XYToIndex(int x, int y);
    
    int MousePosToIndex(int x, int y);
    
    void Render(ShaderProgram *program);
    
    ~Map();
    
public:
    float x,y;
    std::vector<Grid*> grids;
    float size = 1;
    float drawArray[12] = { -0.5f*size, 0.5f*size, 0.5f*size, 0.5f*size, 0.5f*size, -0.5f*size, 0.5f*size, -0.5f*size, -0.5f*size, -0.5f*size, -0.5f*size, 0.5f*size };
    int num_of_grid;
    int height;
    int width;
};
