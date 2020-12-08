
#pragma once
#include "Bullet.h"
#include "Tower.h"


Bullet::Bullet(float x, float y, float size, glm::vec2 direction, float speed, float damage, Tower* parent):
Entity(x,y,size), speed(speed), direction(direction), damage(damage), parent(parent){}

void Bullet::Update(float delta_time, std::vector<Bullet*>& bullets){
    x+=direction.x*speed;
    y+=direction.y*speed;
    
    float distance = glm::distance(glm::vec2(x,y), glm::vec2(parent->x,parent->y));
    if(distance>parent->range) Destroy(bullets);
}

void Bullet::Render(ShaderProgram* program){
    glm::mat4 mv = glm::mat4(1);
    mv = glm::translate(mv, glm::vec3(x,y,0));
    program->SetModelMatrix(mv);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, drawArray);
    glEnableVertexAttribArray(program->positionAttribute);
    program->SetColor(0, 0, 0, 1);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program->positionAttribute);
}


void Bullet::Destroy(std::vector<Bullet*>& bullets){
    for (size_t i=0; i<bullets.size(); i++) {
        if(bullets[i]==this){
            bullets[i] = bullets[bullets.size()-1];
            bullets[bullets.size()-1] = nullptr;
            bullets.pop_back();
            delete this;
            return;
        }
    }
}
