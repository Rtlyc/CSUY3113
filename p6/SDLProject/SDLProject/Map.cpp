#include "Map.h"


Map::Map(unsigned int* data, int width, int height, float x, float y):
x(x),y(y),width(width),height(height),num_of_grid(width*height)
{
    // num_of_grid = width*height
    // 6 vertices create a square
    // 2 floats represent a vertice
    for (int i=0; i<num_of_grid; i++) {
        Grid* cur = new Grid;
        IndexToXY(i, cur->i, cur->j);
        cur->x = x + cur->j*size;
        cur->y = y - cur->i*size;
        cur->type = static_cast<GridType>(data[i]);
        cur->tower = nullptr;
        grids.push_back(cur);

    }
}

void Map::IndexToXY(int index, int& x, int& y){
    x = index/width;
    y = index%width;
}

int Map::XYToIndex(int x, int y){return x*width+y;}

int Map::MousePosToIndex(int x, int y){
    int i,j;
    i = x/64+2;
    if(y<50) j = 0;
    else j = (y-50)/64 + 1;
    return XYToIndex(j,i);
}

void Map::Render(ShaderProgram *program){
    for (int i=0; i<num_of_grid; i++) {
        glm::mat4 mv = glm::mat4(1);
        Grid* cur = grids[i];
        mv = glm::translate(mv, glm::vec3(cur->x,cur->y,0));
        program->SetModelMatrix(mv);
        glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, drawArray);
        glEnableVertexAttribArray(program->positionAttribute);
        program->SetColor(0, 0, 0, 1);
        if(grids[i]->type==GRASS) program->SetColor(0.20, 0.39, 0.12, 1);
        if(grids[i]->type==ROAD)  program->SetColor(0.77, 0.52, 0.38, 1);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(program->positionAttribute);
    }
}

Map::~Map(){
    for (int i=0; i<num_of_grid; i++) {
        delete grids[i];
        grids[i] = nullptr;
    }
}
